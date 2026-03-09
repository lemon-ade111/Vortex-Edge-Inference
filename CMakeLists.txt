cmake_minimum_required(VERSION 3.16)
project(ImageProcessor)

set(CMAKE_CXX_STANDARD 17)

# 寻找依赖
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

# --- 关键修正点：手动设置 ONNX 绝对路径 ---
# 注意：确保你的目录名确实叫 onnx-lib
set(ONNX_DIR "/home/lemon/MySuperProject/onnx-lib")

# 告诉编译器头文件在这个 include 文件夹里
include_directories(${ONNX_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ImageProcessor src/main.cpp)

# 链接库文件：必须指向具体的 .so 文件
target_link_libraries(ImageProcessor 
    ${OpenCV_LIBS} 
    Threads::Threads
    ${ONNX_DIR}/lib/libonnxruntime.so
)