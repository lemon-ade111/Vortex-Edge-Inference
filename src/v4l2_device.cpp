#include "v4l2_device.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iostream>
#include <cstring>
#include <cerrno>

V4L2Device::V4L2Device(std::string path) : dev_path_(std::move(path)) {}

V4L2Device::~V4L2Device() {
    closeDevice();
}

bool V4L2Device::openDevice() {
    fd_ = open(dev_path_.c_str(), O_RDWR);
    if (fd_ < 0) {
        std::cerr << "[CRITICAL] Open " << dev_path_ << " failed: " << std::strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool V4L2Device::probeCapabilities() {
    struct v4l2_capability cap{}; // 使用 {} 确保每一个 bit 都是 0
    if (ioctl(fd_, VIDIOC_QUERYCAP, &cap) < 0) {
        std::cerr << "[ERROR] Query capabilities failed." << std::endl;
        return false;
    }

    // 深度枚举：这不只是打印，这是在确认硬件逻辑
    std::cout << "--- Hardware Reconnaissance ---" << std::endl;
    std::cout << "  Driver: " << cap.driver << std::endl;
    std::cout << "  Bus Info: " << cap.bus_info << std::endl;
    
    // 检查是否具备流式 I/O 能力，这是后续 mmap 的前提
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        std::cerr << "  [FAIL] Device does not support Streaming I/O!" << std::endl;
        return false;
    }
    std::cout << "  [PASS] Streaming I/O supported." << std::endl;
    return true;
}

bool V4L2Device::negotiateFormat(int req_width, int req_height) {
    struct v4l2_format fmt{};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = req_width;
    fmt.fmt.pix.height = req_height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // 我们死磕这个格式
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    std::cout << "[INFO] Requesting format: " << req_width << "x" << req_height << std::endl;

    // VIDIOC_S_FMT 是会“写”数据的系统调用
    if (ioctl(fd_, VIDIOC_S_FMT, &fmt) < 0) {
        std::cerr << "[ERROR] Set Format failed: " << std::strerror(errno) << std::endl;
        return false;
    }

    // --- 核心看穿点：检查内核改动了什么 ---
    if (fmt.fmt.pix.width != req_width || fmt.fmt.pix.height != req_height) {
        std::cout << "[WARN] Kernel negotiated a different resolution: " 
                  << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height << std::endl;
    } else {
        std::cout << "[SUCCESS] Resolution confirmed: " << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height << std::endl;
    }

    std::cout << "  Bytes per line: " << fmt.fmt.pix.bytesperline << std::endl;
    std::cout << "  Image Size: " << fmt.fmt.pix.sizeimage << " bytes" << std::endl;
    
    return true;
}

void V4L2Device::closeDevice() {
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
        std::cout << "[INFO] Device handle released." << std::endl;
    }
}