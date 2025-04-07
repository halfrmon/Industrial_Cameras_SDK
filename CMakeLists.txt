cmake_minimum_required(VERSION 3.10.0)
project(Hikvision_SDK VERSION 0.1.0 LANGUAGES C CXX)
set(INCLUDE_DIRS 
    /opt/MVS/include
    ${CMAKE_SOURCE_DIR}/Include
)

find_package(X11 REQUIRED)
find_package(OpenCV REQUIRED)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 海康威视SDK路径（根据实际安装位置修改）
set(HIK_SDK_INCLUDE_DIR /opt/MVS/include)
set(HIK_SDK_LIB_DIR /opt/MVS/lib/64)


# 自动搜集 src 目录下的所有 .cpp 文件
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 添加可执行文件
add_executable(Hikvision_SDK ${SOURCES})

# 包含头文件路径
target_include_directories(Hikvision_SDK PRIVATE
    ${HIK_SDK_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include 
    ${X11_INCLUDE_DIR}  
    
)

# 链接库
target_link_directories(Hikvision_SDK PRIVATE ${HIK_SDK_LIB_DIR})
target_link_libraries(Hikvision_SDK PRIVATE
    MvCameraControl  # 海康SDK主库
    pthread          # 多线程支持
    ${X11_LIBRARIES}
    ${OpenCV_LIBS} 
)
