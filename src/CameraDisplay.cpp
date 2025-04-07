// CameraDisplay.cpp
#include "CameraDisplay.h"
#include <iostream>

CameraDisplay::CameraDisplay() : m_display(nullptr), m_window(0), m_windowClosed(false) {}

CameraDisplay::~CameraDisplay() {
    if (m_display) {
        XCloseDisplay(m_display);
    }
}

bool CameraDisplay::initialize(int width, int height) {
    // 打开X11显示连接
    m_display = XOpenDisplay(NULL);
    if (!m_display) {
        std::cerr << "Failed to open X11 display" << std::endl;
        return false;
    }

    // 创建窗口
    int screen = DefaultScreen(m_display);
    m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, screen),
                                  0, 0, width, height, 1,
                                  BlackPixel(m_display, screen),
                                  WhitePixel(m_display, screen));

    // 设置窗口标题
    XStoreName(m_display, m_window, "Camera Display");

    // 选择输入事件
    XSelectInput(m_display, m_window, ExposureMask | KeyPressMask | StructureNotifyMask);

    // 显示窗口
    XMapWindow(m_display, m_window);

    // 等待窗口映射完成
    XEvent e;
    while (true) {
        XNextEvent(m_display, &e);
        if (e.type == MapNotify)
            break;
    }

    return true;
}

void CameraDisplay::displayFrame(void* cameraHandle, const MV_FRAME_OUT& frame) {
    if (!m_display || m_windowClosed) return;

    MV_DISPLAY_FRAME_INFO_EX stDisplayInfo = {0};
    stDisplayInfo.nWidth = frame.stFrameInfo.nWidth;
    stDisplayInfo.nHeight = frame.stFrameInfo.nHeight;
    stDisplayInfo.enPixelType = frame.stFrameInfo.enPixelType;
    stDisplayInfo.pImageBuf = frame.pBufAddr;
    stDisplayInfo.nImageBufLen = frame.stFrameInfo.nFrameLen;


    int nRet = MV_CC_DisplayOneFrameEx(cameraHandle, reinterpret_cast<void*>(m_window), &stDisplayInfo);
    if (nRet != MV_OK) {
        std::cerr << "Display frame failed: 0x" << std::hex << nRet << std::endl;
    }

    processEvents();
}

void CameraDisplay::processEvents() {
    if (!m_display) return;

    XEvent e;
    while (XPending(m_display) > 0) {
        XNextEvent(m_display, &e);
        if (e.type == DestroyNotify) {
            m_windowClosed = true;
        }
    }
}