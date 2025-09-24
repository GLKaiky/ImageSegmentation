cmake_minimum_required(VERSION 3.18)

project(ImageSegmentation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

add_executable(${PROJECT_NAME}
    main.cpp
    libs/stb/std_image_impl.cpp 
)

target_include_directories(${PROJECT_NAME} PRIVATE "libs")
