#include "CameraOpener.h"
#include <sstream>
#include <cstdio>

CameraOpener::CameraOpener() : m_handle(nullptr) {}

CameraOpener::~CameraOpener() {
    if (m_handle) {
        // 注意：这里不主动关闭，由CameraCloser负责
    }
}


bool CameraOpener::printCameraInfo(MV_CC_DEVICE_INFO* pstMVDevInfo){
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
        // Print current IP and user defined name
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n" , nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_GENTL_GIGE_DEVICE)
    {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_GENTL_CAMERALINK_DEVICE)
    {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stCMLInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stCMLInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stCMLInfo.chModelName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_GENTL_CXP_DEVICE)
    {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stCXPInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stCXPInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stCXPInfo.chModelName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_GENTL_XOF_DEVICE)
    {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stXoFInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stXoFInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stXoFInfo.chModelName);
    }
    else
    {
        printf("Not support.\n");
    }
    return true;
}



bool CameraOpener::initialize(const std::string& cameraIp, const std::string& netExportIp) {
    m_cameraIp = cameraIp;
    m_netExportIp = netExportIp;
    
    // 1. 初始化SDK
    int nRet = MV_CC_Initialize();
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Initialize SDK failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        return false;
    }
    
    // 2. 创建设备句柄
    if (!createHandle()) {
        return false;
    }
    
    // 3. 打开设备
    if (!openDevice()) {
        return false;
    }
    
    // 4. 设置最优数据包大小
    if (!setPacketSize()) {
        // 这不是致命错误，可以继续
    }
    
    // 5. 设置触发模式为off
    if (!setTriggerMode(true)) {
        return false;
    }
    
    return true;
}

bool CameraOpener::createHandle() {
    MV_CC_DEVICE_INFO_LIST stDevList;
    memset(&stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    // 枚举设备
    int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE | MV_GENTL_CAMERALINK_DEVICE | MV_GENTL_CXP_DEVICE | MV_GENTL_XOF_DEVICE, &stDevList);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Enum Devices failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        return false;
    }
    unsigned int nIndex = 0;
    if(stDevList.nDeviceNum > 0){
        for(int i = 0;i<stDevList.nDeviceNum;i++){
            // 输出设备索引
            std::ostringstream oss;
            oss << "Device[" << i << "]:";
            std::cout << oss.str() << std::endl;
            
            MV_CC_DEVICE_INFO* pstMVDevInfo = stDevList.pDeviceInfo[i];
            if(NULL == pstMVDevInfo){
                break;
            }
            printCameraInfo(pstMVDevInfo);

        }
               // 选择第一个设备创建句柄-->这里需要修改成用户输入索引来进行创建句柄
        MV_CC_DEVICE_INFO* selectedDevice = stDevList.pDeviceInfo[0];
        nRet = MV_CC_CreateHandle(&m_handle, selectedDevice);
        if (MV_OK != nRet) {
            std::ostringstream oss;
            oss << "Create Handle failed! Error code: 0x" << std::hex << nRet;
            m_lastError = oss.str();
            return false;
        }
        return true;
    }

    else{
        std::cout << "Find No Devices!" << std::endl;
        return false;
    }
}
    


    // MV_CC_DEVICE_INFO stDevInfo = {0};
    // MV_GIGE_DEVICE_INFO stGigEDev = {0};
    
    // unsigned int nIp1, nIp2, nIp3, nIp4, nIp;
    // sscanf(m_cameraIp.c_str(), "%d.%d.%d.%d", &nIp1, &nIp2, &nIp3, &nIp4);
    // nIp = (nIp1 << 24) | (nIp2 << 16) | (nIp3 << 8) | nIp4;
    // stGigEDev.nCurrentIp = nIp;
    
    // sscanf(m_netExportIp.c_str(), "%d.%d.%d.%d", &nIp1, &nIp2, &nIp3, &nIp4);
    // nIp = (nIp1 << 24) | (nIp2 << 16) | (nIp3 << 8) | nIp4;
    // stGigEDev.nNetExport = nIp;
    
    // stDevInfo.nTLayerType = MV_GIGE_DEVICE;
    // stDevInfo.SpecialInfo.stGigEInfo = stGigEDev;
    
    // int nRet = MV_CC_CreateHandle(&m_handle, &stDevInfo);
    // if (MV_OK != nRet) {
    //     std::ostringstream oss;
    //     oss << "Create Handle failed! Error code: 0x" << std::hex << nRet;
    //     m_lastError = oss.str();
    //     return false;
    // }
    
    // return true;
// }

bool CameraOpener::openDevice() {
    int nRet = MV_CC_OpenDevice(m_handle);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Open Device failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        return false;
    }
    return true;
}

bool CameraOpener::setPacketSize() {
    int nPacketSize = MV_CC_GetOptimalPacketSize(m_handle);
    if (nPacketSize > 0) {
        int nRet = MV_CC_SetIntValueEx(m_handle, "GevSCPSPacketSize", nPacketSize);
        if(nRet != MV_OK) {
            std::ostringstream oss;
            oss << "Warning: Set Packet Size failed! Error code: 0x" << std::hex << nRet;
            m_lastError = oss.str();
            return false;
        }
    } else {
        std::ostringstream oss;
        oss << "Warning: Get Packet Size failed! Error code: 0x" << std::hex << nPacketSize;
        m_lastError = oss.str();
        return false;
    }
    return true;
}

bool CameraOpener::setTriggerMode(bool off) {
    int nRet = MV_CC_SetEnumValue(m_handle, "TriggerMode", 
                                off ? MV_TRIGGER_MODE_OFF : MV_TRIGGER_MODE_ON);
    if (MV_OK != nRet) {
        std::ostringstream oss;
        oss << "Set Trigger Mode failed! Error code: 0x" << std::hex << nRet;
        m_lastError = oss.str();
        return false;
    }
    return true;
}