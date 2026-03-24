#ifndef V4L2_DEVICE_H
#define V4L2_DEVICE_H

#include <string>
#include <linux/videodev2.h>

struct VideoBuffer {
    void*  start;
    size_t length;
};

class V4L2Device {
public:
    explicit V4L2Device(std::string path);
    ~V4L2Device();

    // Disable copy for hardware safety
    V4L2Device(const V4L2Device&) = delete;
    V4L2Device& operator=(const V4L2Device&) = delete;

    bool openDevice();
    bool probeCapabilities();
    bool negotiateFormat(int width, int height);
    
    // Phase 1.2 Core Methods
    bool initMemoryMapping();
    void unmapMemory();
    
    void closeDevice();

private:
    std::string dev_path_;
    int fd_ = -1;
    
    VideoBuffer* buffers_ = nullptr;
    unsigned int n_buffers_ = 0;

    void handleError(const std::string& prefix);
};

#endif