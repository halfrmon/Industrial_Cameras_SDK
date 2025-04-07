#include "CameraCloser.h"
#include <iostream>

CameraCloser::CameraCloser() {}

CameraCloser::~CameraCloser() {}

bool CameraCloser::close(void*& handle) {
    if (!handle) {
        m_lastError = "Handle is already null";
        return true; // 不算错误，已经关闭了
    }
    
    bool result = true;
    
    // 1. 关闭设备
    int nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Close Device failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        result = false;
    }
    
    // 2. 销毁句柄
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Destroy Handle failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        result = false;
    }
    
    // 3. 反初始化SDK
    MV_CC_Finalize();
    
    // 无论前面是否出错，都要置空handle
    handle = nullptr;
    
    return result;
}