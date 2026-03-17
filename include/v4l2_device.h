#ifndef V4L2_DEVICE_H
#define V4L2_DEVICE_H

#include <string>
#include <linux/videodev2.h>

class V4L2Device {
public:
    explicit V4L2Device(std::string path);
    ~V4L2Device();

    // 严禁拷贝：防止多个对象操作同一个硬件句柄导致系统崩溃
    V4L2Device(const V4L2Device&) = delete;
    V4L2Device& operator=(const V4L2Device&) = delete;

    bool openDevice();
    void closeDevice();
    
    // 任务 1.1 核心方法
    bool probeCapabilities();
    bool negotiateFormat(int req_width, int req_height);

private:
    std::string dev_path_;
    int fd_ = -1;
};

#endif