// ImageSaver.cpp
#include "ImageSaver.h"


ImageSaver::ImageSaver() {
    // 确保捕获目录存在
    ensureDirectoryExists("./captures/RawImages");
    ensureDirectoryExists("./captures/OpencvImages");
}


bool ImageSaver::saveRawImage(const MV_FRAME_OUT& frame, const std::string& directory) {
    std::string filename = generateFilename(".raw");
    std::ofstream file(directory + "/" + filename, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(frame.pBufAddr), frame.stFrameInfo.nFrameLen);
    file.close();
    
    // 保存元数据
    std::ofstream meta(directory + "/" + filename + ".meta");
    meta << "Width: " << frame.stFrameInfo.nWidth << "\n"
         << "Height: " << frame.stFrameInfo.nHeight << "\n"
         << "PixelType: " << frame.stFrameInfo.enPixelType << "\n"
         << "FrameNum: " << frame.stFrameInfo.nFrameNum;
    meta.close();
    
    return true;
}
bool ImageSaver::saveAsOpenCV(const MV_FRAME_OUT& frame, const std::string& directory, bool convertToRGB) {
    // 检查有效性和目录存在
    if (!frame.pBufAddr || frame.stFrameInfo.nFrameLen == 0) {
        std::cerr << "Invalid frame data" << std::endl;
        return false;
    }
    
    if (!ensureDirectoryExists(directory)) {
        std::cerr << "Cannot access directory: " << directory << std::endl;
        return false;
    }

    cv::Mat img;
    bool conversionNeeded = true;

    // 根据像素类型创建OpenCV Mat
    switch(frame.stFrameInfo.enPixelType) {
        // 单通道格式
        case PixelType_Gvsp_Mono8:
            img = cv::Mat(frame.stFrameInfo.nHeight, frame.stFrameInfo.nWidth, 
                         CV_8UC1, frame.pBufAddr);
            break;
            
        case PixelType_Gvsp_Mono10:
        case PixelType_Gvsp_Mono12:
            // 10/12位灰度需要转换为8位
            img = cv::Mat(frame.stFrameInfo.nHeight, frame.stFrameInfo.nWidth,
                         CV_16UC1, frame.pBufAddr);
            img.convertTo(img, CV_8UC1, 255.0/4095.0); // 12->8位缩放
            break;

        // 彩色格式
        case PixelType_Gvsp_RGB8_Packed:
            img = cv::Mat(frame.stFrameInfo.nHeight, frame.stFrameInfo.nWidth, 
                         CV_8UC3, frame.pBufAddr);
            conversionNeeded = convertToRGB;
            break;
            
        case PixelType_Gvsp_BGR8_Packed:
            img = cv::Mat(frame.stFrameInfo.nHeight, frame.stFrameInfo.nWidth, 
                         CV_8UC3, frame.pBufAddr);
            break;
            
        case PixelType_Gvsp_YUV422_Packed:
            // YUV422转BGR
            img = cv::Mat(frame.stFrameInfo.nHeight, frame.stFrameInfo.nWidth,
                         CV_8UC2, frame.pBufAddr);
            cv::cvtColor(img, img, cv::COLOR_YUV2BGR_YUYV);
            break;
            
        case PixelType_Gvsp_BayerRG8:
            // Bayer转BGR
            img = cv::Mat(frame.stFrameInfo.nHeight, frame.stFrameInfo.nWidth,
                         CV_8UC1, frame.pBufAddr);
            cv::cvtColor(img, img, cv::COLOR_BayerRG2BGR);
            break;

        default:
            std::cerr << "Unsupported pixel type: 0x" 
                      << std::hex << frame.stFrameInfo.enPixelType 
                      << ", saving as RAW" << std::endl;
            return saveRawImage(frame, directory);
    }

    // 执行必要的颜色空间转换
    if (conversionNeeded) {
        cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
    }

    // 生成文件名并保存
    std::string filename = generateFilename(".png");
    std::string fullPath = directory + "/" + filename;
    
    try {
        bool success = cv::imwrite(fullPath, img);
        if (!success) {
            std::cerr << "Failed to write image: " << fullPath << std::endl;
            return false;
        }
        std::cout << "Image saved: " << fullPath << std::endl;
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception: " << e.what() << std::endl;
        return false;
    }
}

std::string ImageSaver::generateFilename(const std::string& ext) const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), m_format.c_str());
    ss << "_" << std::setfill('0') << std::setw(4) << m_counter++;
    ss << ext;
    
    return ss.str();
}

bool ImageSaver::ensureDirectoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return mkdir(path.c_str(), 0755) == 0;
    }
    return (info.st_mode & S_IFDIR) != 0;
}