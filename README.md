# Vortex-Edge-Inference 🌪️

[![C++](https://img.shields.io/badge/Language-C++17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**Vortex-Edge-Inference** is a high-performance, asynchronous vision inference framework designed for edge computing environments. Built with Modern C++ (C++17), it focuses on maximizing throughput and minimizing latency on resource-constrained devices.

> **Vortex-Edge-Inference** 是一个为边缘计算环境设计的高性能、异步视觉推理框架。基于现代 C++ (C++17) 构建，致力于在资源受限的设备上最大化吞吐量并最小化延迟。

---

## 🚀 Key Features | 核心特性

- **Asynchronous Producer-Consumer Architecture**: Decouples image acquisition and AI inference to ensure zero-frame-drop acquisition.
  - **多线程生产者-消费者架构**: 解耦图像采集与 AI 推理，确保采集端零丢帧。
- **Fine-grained Resource Control**: Leverages `std::condition_variable` and `std::mutex` for efficient thread synchronization and flow control.
  - **精细资源管控**: 利用条件变量与互斥锁实现高效的线程同步与流控，防止内存溢出 (OOM)。
- **Modern C++ Memory Management**: Utilizes Move Semantics and RAII to minimize memory copy overhead.
  - **现代 C++ 内存管理**: 深度使用移动语义与 RAII 机制，大幅降低图像数据拷贝开销。
- **ONNX Runtime Integration**: Ready for edge AI deployment with optimized model inference.
  - **ONNX Runtime 集成**: 针对边缘 AI 部署进行了推理链条优化。

---

## 🏗️ Architecture | 系统架构

```text
[ Camera/Source ] -> [ Producer Thread ] -> [ Thread-Safe Queue (Max 5) ] -> [ Consumer Threads (Worker Pool) ] -> [ ONNX Inference ]