#include "v4l2_device.h"
#include <iostream>

int main() {
    V4L2Device camera("/dev/video0");

    if (!camera.openDevice()) return 1;

    // 进行深度探测
    if (!camera.probeCapabilities()) return 1;

    // 故意请求一个奇怪的分辨率，看内核怎么“教育”你
    // 比如请求 1000x1000，看它会不会给你修成 960x720 或 640x480
    camera.negotiateFormat(1000, 1000);

    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}