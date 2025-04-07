#ifndef CAMERA_OPENER_H
#define CAMERA_OPENER_H

#include <string.h>
#include <iostream>
#include "MvCameraControl.h"

/**
 * @class CameraOpener
 * @brief 负责相机的初始化和打开操作
 * 
 * 这个类封装了相机的初始化、创建设备句柄和打开设备的操作，
 * 为后续的抓取操作做好准备。
 */
class CameraOpener {
public:
    CameraOpener();
    ~CameraOpener();

    /**
     * @brief 初始化相机SDK和相机设备
     * @param cameraIp 相机IP地址
     * @param netExportIp 网卡IP地址
     * @return 是否初始化成功
     */
    bool initialize(const std::string& cameraIp, const std::string& netExportIp);
    
    /**
     * @brief 获取相机句柄
     * @return 相机操作句柄
     */
    void* getHandle() const { return m_handle; }
    
    /**
     * @brief 获取错误信息
     * @return 最后一次操作的错误信息
     */
    std::string getLastError() const { return m_lastError; }

    /**
     * @brief 打印相机信息
     * @return 是否打印成功 
     */
    bool printCameraInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);

private:
    /**
     * @brief 创建设备句柄
     * @return 是否创建成功
     */
    bool createHandle();
    
    /**
     * @brief 打开设备
     * @return 是否打开成功
     */
    bool openDevice();
    
    /**
     * @brief 设置最优数据包大小
     * @return 是否设置成功
     */
    bool setPacketSize();
    
    /**
     * @brief 设置触发模式
     * @param off 是否关闭触发模式
     * @return 是否设置成功
     */
    bool setTriggerMode(bool off);

    void* m_handle;                 ///< 相机操作句柄
    std::string m_lastError;        ///< 最后一次操作的错误信息
    std::string m_cameraIp;         ///< 相机IP地址
    std::string m_netExportIp;      ///< 网卡IP地址
    MV_CC_DEVICE_INFO* pstMVDevInfo; ///< 输出设备信息指针

};

#endif // CAMERA_OPENER_H