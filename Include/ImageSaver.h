// ImageSaver.h
#ifndef IMAGE_SAVER_H
#define IMAGE_SAVER_H

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <sys/stat.h> // 
#include <opencv2/opencv.hpp>
#include <fstream>
#include "MvCameraControl.h"

/**
 * @brief  图像保存器
 *
 * 用于保存相机捕获的图像数据，可以选择保存为原始数据或OpenCV格式。
 * 支持多种保存格式，如JPEG、PNG、BMP等。
 * 支持自动生成文件名，包括时间戳和序号。
 * 支持目录创建和权限检查。
 * 
 */
class ImageSaver {
public:
    ImageSaver();
    
    // 保存原始图像数据
    bool saveRawImage(const MV_FRAME_OUT& frame, const std::string& directory = "./captures/RawImages");
    
    // 保存为OpenCV格式（需链接OpenCV）
    bool saveAsOpenCV(const MV_FRAME_OUT& frame, const std::string& directory = "./captures/OpencvImages", bool convertToRGB = true);
    
    // 设置保存格式
    void setSaveFormat(const std::string& format) { m_format = format; }
    
private:
    std::string generateFilename(const std::string& ext) const; // 生成文件名
    bool ensureDirectoryExists(const std::string& path); // 确保目录存在
    
    std::string m_format = "%Y%m%d_%H%M%S";
    mutable int m_counter = 0;
};

#endif // IMAGE_SAVER_H