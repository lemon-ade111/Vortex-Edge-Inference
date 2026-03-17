# Vortex-Edge-Inference

High-performance vision middleware for Linux edge devices.

## [2026-03-17] Phase 1.1: Hardware Negotiation

### 1. Implementation
- Encapsulated `V4L2Device` using C++17 RAII.
- Handled kernel-level communication via native `ioctl` system calls.
- Implemented error diagnosis through `errno` and `strerror`.

### 2. Kernel Handshake Results
- **Interface**: V4L2 (Video for Linux Two)
- **Pixel Format**: YUYV 4:2:2
- **Resolution Sync**: 
    - Requested: 1000x1000
    - Negotiated: **1280x720** (Synchronized by hardware driver)
- **Memory Metrics**:
    - Bytesperline (Stride): 2560 bytes
    - Sizeimage: **1,843,200 bytes**

### 3. Verification
- Device node `/dev/video0` successfully acquired with file descriptor index 3.
- Streaming I/O capability validated through `v4l2_capability`.

---
*Status: Hardware handshake established. Base infrastructure initialized.*
