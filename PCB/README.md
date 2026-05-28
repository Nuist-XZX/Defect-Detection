# PCB Defect Detection using YOLOv5 6.0 with ONNX Runtime (Windows)

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![ONNX Runtime](https://img.shields.io/badge/ONNX%20Runtime-1.14.1-orange.svg)](https://onnxruntime.ai/)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.10.0-green.svg)](https://opencv.org/)
[![YOLOv5](https://img.shields.io/badge/YOLOv5-6.0-red.svg)](https://github.com/ultralytics/yolov5)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)]()

基于 **YOLOv5 6.0** 与 **ONNX Runtime 1.14.1** 的 PCB 缺陷检测 C++ 实现，专为 **Windows** 平台优化。支持 CPU / GPU 推理，动态输入尺寸，内置 NMS 后处理。

## 📁 目录结构
```text
PCB/
├── data/
│   └── class.names        # 类别名称文件（一行一个类别）
├── include/
│   └── yolov5.h           # YOLO 检测器头文件
├── src/
│   └── yolov5.cpp         # YOLO 检测器实现
├── CMakeLists.txt         # CMake 构建脚本
├── main.cpp               # 主程序入口
└── README.md              # 本文件
```


## ✨ 功能特性

- 🚀 基于 **ONNX Runtime 1.14.1** 推理，支持 DirectML / CUDA 后端
- 📐 支持动态输入尺寸（YOLOv5 6.0 导出的 ONNX 模型默认支持）
- 🧠 内置 LetterBox 预处理 + NMS 后处理
- 🎨 使用 OpenCV 4.10.0 进行图像绘制与显示
- 🧩 单文件头文件设计，易于集成

## 🔧 环境要求（Windows）

| 组件            | 版本                       |
| --------------- | -------------------------- |
| 操作系统        | Windows 10 / 11            |
| 编译器          | Visual Studio 2019/2022 (MSVC) |
| CMake           | 3.10+                      |
| OpenCV          | 4.10.0                     |
| ONNX Runtime    | 1.14.1                     |
| YOLOv5          | 6.0                        |

## 📦 安装指南（Windows）

### 1. 安装 OpenCV 4.10.0

- 从 [OpenCV 官网](https://opencv.org/releases/) 下载 `opencv-4.10.0-windows.exe`
- 运行安装程序，解压到指定目录（例如 `C:\opencv`）
- 将 `C:\opencv\build\x64\vc15\bin` 添加到系统环境变量 `PATH`

### 2. 安装 ONNX Runtime 1.14.1

- 从 [ONNX Runtime Releases](https://github.com/microsoft/onnxruntime/releases/tag/v1.14.1) 下载：
  - **CPU 版本**：`onnxruntime-win-x64-1.14.1.zip`
  - **GPU 版本 (CUDA)**：`onnxruntime-win-x64-gpu-1.14.1.zip`（需已安装 CUDA 和 cuDNN）
- 解压到指定目录（例如 `C:\onnxruntime`）
- 将 `C:\onnxruntime\lib` 添加到系统环境变量 `PATH`

### 3. 安装 CMake

- 从 [CMake 官网](https://cmake.org/download/) 下载 Windows 安装包并安装
- 确保 `cmake` 命令可用

### 4. 准备 YOLOv5 6.0 ONNX 模型

```bash
git clone https://github.com/ultralytics/yolov5.git
cd yolov5
git checkout v6.0
pip install -r requirements.txt
python export.py --weights best.pt --include onnx --opset 12