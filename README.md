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

## [2026-03-19] Phase 1.2: Zero-copy Memory Bridge (mmap)

### 1. Technical Implementation
- **Memory Mapping**: Bridged the gap between kernel-space video buffers and user-space memory via `mmap`.
- **Buffer Pool**: Initialized a 4-slot circular buffer strategy to balance latency and throughput.
- **Resource Management**: Implemented `unmapMemory()` to ensure zero memory leaks during hardware hot-swapping or crashes.

### 2. Physical Auditing Results
- **Memory Blocks**: 4 independent buffers mapped.
- **Mapping Mode**: `rw-s` (Read/Write Shared) verified via `pmap`.
- **Latency Logic**: Eliminated the CPU overhead of traditional `read()` calls, achieving a pure zero-copy pipeline.

### 3. Debugging Insights
- Successfully diagnosed and resolved `EBUSY` conflicts caused by file descriptor stacking.
- Verified that `mmap` length must strictly align with the kernel-negotiated `sizeimage`.

---
*Status: Zero-copy bridge is live. Ready for Task 1.3: Pixel Streaming.*