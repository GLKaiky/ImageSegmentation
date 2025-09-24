cmake_minimum_required(VERSION 3.18)

project(ImageSegmentation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

find_package(OpenCV REQUIRED)
add_executable(segmentate main.cpp)

target_link_libraries(
    OpenCV::opencv_core
    OpenCV::opencv_imgproc
    OpenCV::opencv_highgui
    OpenCV::opencv_imgcodecs
)