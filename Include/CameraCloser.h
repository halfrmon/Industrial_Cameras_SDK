#ifndef CAMERA_CLOSER_H
#define CAMERA_CLOSER_H
#include <string>
#include <sstream>
#include "MvCameraControl.h"

/**
 * @class CameraCloser
 * @brief 负责相机的关闭和资源释放
 * 
 * 这个类封装了相机的关闭、销毁句柄和SDK反初始化的操作，
 * 确保相机资源被正确释放。
 */
class CameraCloser {
public:
    CameraCloser();
    ~CameraCloser();

    /**
     * @brief 关闭相机并释放资源
     * @param handle 相机操作句柄的引用(关闭后会置为nullptr)
     * @return 是否关闭成功
     */
    bool close(void*& handle);
    
    /**
     * @brief 获取错误信息
     * @return 最后一次操作的错误信息
     */
    std::string getLastError() const { return m_lastError; }

private:
    std::string m_lastError;  ///< 最后一次操作的错误信息
};

#endif // CAMERA_CLOSER_H