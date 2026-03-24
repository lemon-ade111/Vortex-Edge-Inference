#include "v4l2_device.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <iostream>
#include <cstring>
#include <cerrno>

V4L2Device::V4L2Device(std::string path) : dev_path_(std::move(path)) {}

V4L2Device::~V4L2Device() {
    closeDevice();
}

void V4L2Device::handleError(const std::string& prefix) {
    std::cerr << "[Vortex-Debug] " << prefix << " failed: " 
              << std::strerror(errno) << " (Errno: " << errno << ")" << std::endl;
}

bool V4L2Device::openDevice() {
    fd_ = open(dev_path_.c_str(), O_RDWR);
    if (fd_ < 0) {
        handleError("open");
        return false;
    }
    return true;
}

bool V4L2Device::probeCapabilities() {
    struct v4l2_capability cap{};
    if (ioctl(fd_, VIDIOC_QUERYCAP, &cap) < 0) {
        handleError("VIDIOC_QUERYCAP");
        return false;
    }
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        std::cerr << "[CRITICAL] Device does not support mmap streaming." << std::endl;
        return false;
    }
    return true;
}

bool V4L2Device::negotiateFormat(int width, int height) {
    struct v4l2_format fmt{};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd_, VIDIOC_S_FMT, &fmt) < 0) {
        handleError("VIDIOC_S_FMT");
        return false;
    }
    return true;
}

bool V4L2Device::initMemoryMapping() {
    // 1. Request kernel buffers
    struct v4l2_requestbuffers req{};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd_, VIDIOC_REQBUFS, &req) < 0) {
        handleError("VIDIOC_REQBUFS");
        return false;
    }

    buffers_ = new VideoBuffer[req.count];
    n_buffers_ = req.count;

    // 2. Map each buffer into user space
    for (unsigned int i = 0; i < n_buffers_; ++i) {
        struct v4l2_buffer buf{};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd_, VIDIOC_QUERYBUF, &buf) < 0) {
            handleError("VIDIOC_QUERYBUF");
            return false;
        }

        buffers_[i].length = buf.length;
        buffers_[i].start = mmap(
            NULL, 
            buf.length, 
            PROT_READ | PROT_WRITE, 
            MAP_SHARED, 
            fd_, 
            buf.m.offset
        );

        if (buffers_[i].start == MAP_FAILED) {
            handleError("mmap");
            return false;
        }
        std::cout << "[INFO] Buffer " << i << " mapped at: " << buffers_[i].start << std::endl;
    }
    return true;
}

void V4L2Device::unmapMemory() {
    if (buffers_) {
        for (unsigned int i = 0; i < n_buffers_; ++i) {
            if (buffers_[i].start && buffers_[i].start != MAP_FAILED) {
                munmap(buffers_[i].start, buffers_[i].length);
            }
        }
        delete[] buffers_;
        buffers_ = nullptr;
        std::cout << "[INFO] Unmapped all buffers." << std::endl;
    }
}

void V4L2Device::closeDevice() {
    unmapMemory();
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}