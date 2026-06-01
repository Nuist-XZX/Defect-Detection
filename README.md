# Defect-Detection
- 缺陷检测项目：基于 YOLOv5 与 OpenCV/ONNX Runtime 的 C/C++ 实现
- 缺陷类型：short短路、missing_hole漏孔、spurious_copper杂铜、mouse_bite鼠标咬伤、open_circuit开路、spur毛刺/杂散

---

## 📂 项目结构说明

本项目为工业缺陷检测工程模块。

```text
Defect-Detection/
├── PCB/                # 基于 Onnx Runtime 的模型部署实现
├── opencvdnn/          # 基于 OpenCV DNN 的模型部署实现
├── yolov5/             # YOLOv5 模型训练、导出与转换脚本
├── .gitignore          # Git忽略文件配置
└── README.md           # 项目说明文档
```

## 1. yolov5/ 目录

- **目标**：完成缺陷检测模型的训练与导出  
- **核心工作流**：  
  - **模型训练**：使用 PCB 缺陷数据集训练 YOLOv5 模型  
  - **模型导出**：将训练好的 `.pt` 模型导出为 `.onnx` 格式  
  - **模型验证**：验证导出的 ONNX 模型在推理框架中的正确性  
- **输出产物**：训练好的权重文件、ONNX 模型文件  

## 2. PCB/ 目录 ✅（当前唯一可运行的部署实现）

- **目标**：基于 ONNX Runtime 实现 YOLOv5 模型的 C++ 部署  
- **功能**：  
  - 加载 ONNX 格式的 YOLOv5 模型  
  - 实现图像预处理（letterbox、归一化、通道转换等）  
  - 模型推理与后处理（NMS、坐标还原、置信度过滤）  
  - 缺陷检测结果可视化（画框、保存结果）  
- **说明**：这是目前项目中唯一成功实现并可运行的部署方案。  

## 3. opencvdnn/ 目录 ⚠️（部署尚未成功实现）

- **目标**：基于 OpenCV DNN 模块实现 YOLOv5 模型的 C++ 部署  
- **当前状态**：  
  - 代码框架已搭建，但模型推理 / 后处理环节尚未调试通过  
  - 后续将参考 `PCB` 目录的实现逻辑，完成 OpenCV DNN 版本的适配与验证  