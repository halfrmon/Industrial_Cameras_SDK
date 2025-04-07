// main.cpp
#include <iostream>
#include <memory>
#include <atomic>
#include <csignal>
#include <sys/select.h>
#include <unistd.h>
#include <thread>

#include "../Include/CameraOpener.h"
#include "../Include/CameraGrabber.h"
#include "../Include/CameraCloser.h"
#include "../Include/ImageSaver.h"
#include "../Include/CameraDisplay.h"


std::atomic<bool> g_saveNextFrame(false);
std::atomic<bool> g_continuousSave(false);

void handleUserInput() {
    std::string input;
    while (true) {
        std::cout << "\nCommand (s: save one, c: toggle continuous save, q: quit): ";
        std::getline(std::cin, input);
        
        if (input == "s") {
            g_saveNextFrame = true;
        } else if (input == "c") {
            g_continuousSave = !g_continuousSave;
            std::cout << "Continuous save " << (g_continuousSave ? "ENABLED" : "DISABLED") << std::endl;
        } else if (input == "q") {
            raise(SIGINT); // 触发退出
            break;
        }
    }
}


int main() {
    
    std::string cameraIp, netExportIp;
    
    std::cout << "Please input Camera IP: ";
    std::cin >> cameraIp;
    
    std::cout << "Please input Net Export IP: ";
    std::cin >> netExportIp;
    std::cin.ignore(); // 清除换行符
    
    // 初始化组件
    CameraDisplay display;
    if (!display.initialize()) {
        std::cerr << "Failed to initialize display" << std::endl;
        return -1;
    }
    
    CameraOpener opener;
    if (!opener.initialize(cameraIp, netExportIp)) {
        std::cerr << "Camera init failed: " << opener.getLastError() << std::endl;
        return -1;
    }
    
    ImageSaver saver;
    saver.setSaveFormat("%Y%m%d_%H%M%S_%04d"); // 设置文件名格式
    
    // 启动用户输入线程
    std::thread inputThread(handleUserInput);
    inputThread.detach();
    
    // 帧回调函数
    auto frameCallback = [&](const MV_FRAME_OUT& frame) {
        // 显示图像
        display.displayFrame(opener.getHandle(), frame);
        
        // 保存图像条件判断
        if (g_saveNextFrame || g_continuousSave) {
            bool OpenCVsaveResult = saver.saveAsOpenCV(frame);
            bool RawsaveResult = saver.saveRawImage(frame);
            if (OpenCVsaveResult) {
                std::cout << "OpenCV_Image saved" << std::endl;
            } else {
                std::cerr << "Failed to save OpenCV_Image" << std::endl;
            }
            if (RawsaveResult) {
                std::cout << "Raw_Image saved" << std::endl;
            }  else {
                std::cerr << "Failed to save Raw_Image" << std::endl;
            }
            g_saveNextFrame = false;
        }
    };
    
    // 开始采集
    CameraGrabber grabber;
    if (!grabber.start(opener.getHandle(), frameCallback)) {
        std::cerr << "Start capture failed: " << grabber.getLastError() << std::endl;
        return -1;
    }
    
    std::cout << "Camera started. Commands:\n"
              << "  s - Save current frame\n"
              << "  c - Toggle continuous save\n"
              << "  q - Quit\n";
    
    // 主循环
    while (!display.isWindowClosed()) {
        display.processEvents();
        usleep(10000); // 10ms
    }
    
    // 停止采集
    grabber.stop();
    
    // 关闭相机
    CameraCloser closer;
    auto Handle = opener.getHandle();
    closer.close(Handle);
    
    std::cout << "Application exited normally." << std::endl;
    return 0;
}