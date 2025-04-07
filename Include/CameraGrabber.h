#ifndef CAMERA_GRABBER_H
#define CAMERA_GRABBER_H

#include <functional>
#include <string>
#include <sstream>
#include <pthread.h>
#include "MvCameraControl.h"

// 定义帧回调函数类型
using FrameCallback = std::function<void(const MV_FRAME_OUT&)>;

/**
 * @class CameraGrabber
 * @brief 负责相机的图像抓取操作
 * 
 * 这个类封装了相机开始抓取、停止抓取以及帧数据回调的功能，
 * 使用独立线程进行图像采集。
 */
class CameraGrabber {
public:
    CameraGrabber();
    ~CameraGrabber();

    /**
     * @brief 开始图像抓取
     * @param handle 相机操作句柄
     * @param callback 帧数据回调函数
     * @return 是否开始成功
     */
    bool start(void* handle, FrameCallback callback = nullptr);
    
    /**
     * @brief 停止图像抓取
     * @return 是否停止成功
     */
    bool stop();
    
    /**
     * @brief 检查是否正在抓取
     * @return 是否正在抓取
     */
    bool isGrabbing() const { return m_isGrabbing; }
    
    /**
     * @brief 获取错误信息
     * @return 最后一次操作的错误信息
     */
    std::string getLastError() const { return m_lastError; }

private:
    /**
     * @brief 工作线程函数
     * @param pUser 用户数据指针
     * @return 线程返回值
     */
    static void* workThread(void* pUser);

    void* m_handle;                 ///< 相机操作句柄
    bool m_isGrabbing;              ///< 是否正在抓取
    std::string m_lastError;        ///< 最后一次操作的错误信息
    FrameCallback m_frameCallback;  ///< 帧数据回调函数
    pthread_t m_threadId;           ///< 工作线程ID
};

#endif // CAMERA_GRABBER_H