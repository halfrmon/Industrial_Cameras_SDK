#include "CameraGrabber.h"
#include <iostream>

CameraGrabber::CameraGrabber() 
    : m_handle(nullptr), 
      m_isGrabbing(false),
      m_threadId(0) {}

CameraGrabber::~CameraGrabber() {
    if (m_isGrabbing) {
        stop();
    }
}

bool CameraGrabber::start(void* handle, FrameCallback callback) {
    if (m_isGrabbing) {
        m_lastError = "Camera is already grabbing";
        return false;
    }
    
    if (!handle) {
        m_lastError = "Invalid camera handle";
        return false;
    }
    
    m_handle = handle;
    m_frameCallback = callback;
    
    // 开始抓取图像
    int nRet = MV_CC_StartGrabbing(m_handle);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Start Grabbing failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        return false;
    }
    
    // 创建工作线程
    m_isGrabbing = true;
    int ret = pthread_create(&m_threadId, NULL, workThread, this);
    if (ret != 0) {
        m_isGrabbing = false;
        m_lastError = "Failed to create worker thread";
        return false;
    }
    
    return true;
}

bool CameraGrabber::stop() {
    if (!m_isGrabbing) {
        m_lastError = "Camera is not grabbing";
        return false;
    }
    
    // 设置停止标志
    m_isGrabbing = false;
    
    // 等待工作线程结束
    pthread_join(m_threadId, NULL);
    
    // 停止抓取
    int nRet = MV_CC_StopGrabbing(m_handle);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Stop Grabbing failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        return false;
    }
    
    return true;
}

void* CameraGrabber::workThread(void* pUser) {
    CameraGrabber* pThis = static_cast<CameraGrabber*>(pUser);
    
    int nRet = MV_OK;
    MV_FRAME_OUT stImageInfo = {0};
    
    while(pThis->m_isGrabbing) {
        nRet = MV_CC_GetImageBuffer(pThis->m_handle, &stImageInfo, 1000);
        if (nRet == MV_OK) {
            if (pThis->m_frameCallback) {
                pThis->m_frameCallback(stImageInfo);
            }
            MV_CC_FreeImageBuffer(pThis->m_handle, &stImageInfo);
        } else {
            if (pThis->m_isGrabbing) {
                std::cerr << "No data[0x" << std::hex << nRet << "]" << std::endl;
            }
            break;
        }
    }
    
    return nullptr;
}