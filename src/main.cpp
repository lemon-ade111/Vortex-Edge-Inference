#include "v4l2_device.h"
#include <iostream>

int main() {
    V4L2Device camera("/dev/video0");

    if (!camera.openDevice()) return -1;
    if (!camera.probeCapabilities()) return -1;
    if (!camera.negotiateFormat(1280, 720)) return -1;

    // Establishing the zero-copy wormhole
    if (!camera.initMemoryMapping()) {
        return -1;
    }

    std::cout << "\nMapping verified. Process idling for audit (pmap)..." << std::endl;
    std::cout << "Press Enter to release resources and exit." << std::endl;
    std::cin.get();

    return 0;
}