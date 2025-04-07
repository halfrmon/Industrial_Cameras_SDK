// CameraDisplay.h
#ifndef CAMERA_DISPLAY_H
#define CAMERA_DISPLAY_H

#include <X11/Xlib.h>
#include "MvCameraControl.h"

/**
 * @brief         相机图像显示类
 *
 * 用于显示相机捕获的图像数据，使用X11窗口进行显示。
 * 支持图像的实时显示和窗口关闭检测。
 *          
 */


class CameraDisplay {
public:
    CameraDisplay();
    ~CameraDisplay();
    
    bool initialize(int width = 800, int height = 600); // 初始化窗口
    void displayFrame(void* cameraHandle, const MV_FRAME_OUT& frame); // 显示图像
    bool isWindowClosed() const { return m_windowClosed; } // 检查窗口是否关闭
    void processEvents(); // 处理事件

private:
    Display* m_display;
    Window m_window;
    bool m_windowClosed;
};

#endif // CAMERA_DISPLAY_H