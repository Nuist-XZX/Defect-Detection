# 8:1:1
# import random
# import shutil
# from pathlib import Path
#
# def split_dataset(
#     images_dir: str,
#     labels_dir: str,
#     output_root: str,
#     train_ratio: float = 0.8,
#     val_ratio: float = 0.1,
#     test_ratio: float = 0.1,
#     img_ext: str = ".jpg",
#     label_ext: str = ".txt",
#     random_seed: int = 42
# ):
#     """
#     将图片和标签按比例随机划分为 train/val/test 并复制到新目录。
#
#     Args:
#         images_dir: 图片文件夹路径
#         labels_dir: 标签文件夹路径
#         output_root: 输出根目录（将创建 train/val/test 子目录）
#         train_ratio: 训练集比例
#         val_ratio: 验证集比例
#         test_ratio: 测试集比例
#         img_ext: 图片文件扩展名（含点）
#         label_ext: 标签文件扩展名（含点）
#         random_seed: 随机种子，保证可复现
#     """
#     assert abs(train_ratio + val_ratio + test_ratio - 1.0) < 1e-6, "比例之和必须为 1"
#
#     random.seed(random_seed)
#
#     # 获取所有图片文件的基础名称（不含扩展名）
#     images_path = Path(images_dir)
#     label_path = Path(labels_dir)
#
#     img_files = list(images_path.glob(f"*{img_ext}"))
#     if not img_files:
#         raise FileNotFoundError(f"在 {images_dir} 中未找到任何 {img_ext} 文件")
#
#     # 提取基础名称并验证标签文件是否存在
#     valid_basenames = []
#     for img_file in img_files:
#         basename = img_file.stem  # 不带扩展名的文件名
#         label_file = label_path / f"{basename}{label_ext}"
#         if label_file.exists():
#             valid_basenames.append(basename)
#         else:
#             print(f"警告: 图片 {img_file.name} 缺少对应的标签文件 {label_file.name}，已跳过")
#
#     if not valid_basenames:
#         raise RuntimeError("没有找到任何匹配的图片-标签对，请检查文件夹和扩展名设置")
#
#     print(f"找到 {len(valid_basenames)} 对有效的图片-标签文件")
#
#     # 随机打乱
#     random.shuffle(valid_basenames)
#
#     # 计算分割点
#     total = len(valid_basenames)
#     train_end = int(total * train_ratio)
#     val_end = train_end + int(total * val_ratio)
#
#     train_names = valid_basenames[:train_end]
#     val_names = valid_basenames[train_end:val_end]
#     test_names = valid_basenames[val_end:]
#
#     print(f"训练集: {len(train_names)} 对")
#     print(f"验证集: {len(val_names)} 对")
#     print(f"测试集: {len(test_names)} 对")
#
#     # 创建输出目录结构
#     subsets = ["train", "val", "test"]
#     for subset in subsets:
#         (Path(output_root) / subset / "images").mkdir(parents=True, exist_ok=True)
#         (Path(output_root) / subset / "labels").mkdir(parents=True, exist_ok=True)
#
#     # 复制文件的辅助函数
#     def copy_pair(basename, subset):
#         src_img = images_path / f"{basename}{img_ext}"
#         dst_img = Path(output_root) / subset / "images" / f"{basename}{img_ext}"
#         shutil.copy2(src_img, dst_img)
#
#         src_label = label_path / f"{basename}{label_ext}"
#         dst_label = Path(output_root) / subset / "labels" / f"{basename}{label_ext}"
#         shutil.copy2(src_label, dst_label)
#
#     # 复制训练集
#     for name in train_names:
#         copy_pair(name, "train")
#     print("训练集复制完成")
#
#     # 复制验证集
#     for name in val_names:
#         copy_pair(name, "val")
#     print("验证集复制完成")
#
#     # 复制测试集
#     for name in test_names:
#         copy_pair(name, "test")
#     print("测试集复制完成")
#
#     print(f"数据集划分完成！结果保存在: {output_root}")
#
# if __name__ == "__main__":
#     # ====== 请根据实际情况修改以下参数 ======
#     IMAGES_DIR = r"C:\Users\admin\Desktop\PCB\pcb_datasets"    # PNG图片文件夹路径
#     LABELS_DIR = r"C:\Users\admin\Desktop\PCB\pcb_txt"        # 标签文件夹路径
#     OUTPUT_ROOT = r"C:\Users\admin\Desktop\PCB"    # 输出根目录
#     # ======================================
#
#     split_dataset(
#         images_dir=IMAGES_DIR,
#         labels_dir=LABELS_DIR,
#         output_root=OUTPUT_ROOT,
#         train_ratio=0.8,
#         val_ratio=0.1,
#         test_ratio=0.1,
#         img_ext=".jpg",    # 如果图片不是 .png 可修改
#         label_ext=".txt",  # 如果标签不是 .txt 可修改（例如 .png 或 .json）
#         random_seed=42
#     )

# xml - txt
# import os
# import glob
# import xml.etree.ElementTree as ET
#
#
# def get_classes(classes_path):
#     with open(classes_path, encoding='utf-8') as f:
#         class_names = f.readlines()
#     class_names = [c.strip() for c in class_names]
#     print("Classes loaded:", class_names)
#     return class_names, len(class_names)
#
#
# def convert(size, box):
#     dw = 1.0 / size[0]
#     dh = 1.0 / size[1]
#     x = (box[0] + box[1]) / 2.0
#     y = (box[2] + box[3]) / 2.0
#     w = box[1] - box[0]
#     h = box[3] - box[2]
#     x = x * dw
#     w = w * dw
#     y = y * dh
#     h = h * dh
#     return (x, y, w, h)
#
#
# def convert_xml_to_yolo(xml_root_path, txt_save_path, classes_path):
#     print("XML root path:", xml_root_path)
#     print("TXT save path:", txt_save_path)
#     print("Classes path:", classes_path)
#
#     if not os.path.exists(txt_save_path):
#         os.makedirs(txt_save_path)
#     print("Directory created:", txt_save_path)
#
#     xml_paths = glob.glob(os.path.join(xml_root_path, '*.xml'))
#     print("XML files found:", xml_paths)
#
#     classes, _ = get_classes(classes_path)
#
#     for xml_id in xml_paths:
#         print("Processing file:", xml_id)
#         txt_id = os.path.join(txt_save_path, os.path.basename(xml_id)[:-4] + '.txt')
#         txt = open(txt_id, 'w')
#         xml = open(xml_id, encoding='utf-8')
#         tree = ET.parse(xml)
#         root = tree.getroot()
#         size = root.find('size')
#         w = int(size.find('width').text)
#         h = int(size.find('height').text)
#         for obj in root.iter('object'):
#             difficult = 0
#             if obj.find('difficult') is not None:
#                 difficult = obj.find('difficult').text
#             cls = obj.find('name').text
#             print("Class found:", cls)
#             if cls not in classes or int(difficult) == 1:
#                 continue
#             cls_id = classes.index(cls)
#             xmlbox = obj.find('bndbox')
#             b = (int(float(xmlbox.find('xmin').text)), int(float(xmlbox.find('xmax').text)),
#                  int(float(xmlbox.find('ymin').text)), int(float(xmlbox.find('ymax').text)))
#             box = convert((w, h), b)
#             txt.write(str(cls_id) + ' ' + ' '.join([str(a) for a in box]) + '\n')
#         txt.close()
#         print("TXT file created:", txt_id)
#
#
# if __name__ == '__main__':
#     # 用户输入XML文件路径和TXT文件存放路径
#     xml_root_path = r"C:\Users\admin\Desktop\PCB\pcb_xml"     # 替换为你的XML文件路径
#     txt_save_path = r"C:\Users\admin\Desktop\PCB\pcb_txt"     # 替换为你的TXT文件存放路径
#     classes_path = r"C:\Users\admin\Desktop\PCB\class.txt"    # 替换为你的类别文件路径
#     convert_xml_to_yolo(xml_root_path, txt_save_path, classes_path)

# from pathlib import Path
#
# path1 = r'C:\Users\admin\Desktop\470\labels'
# path = Path(path1)
# print(path)
# print(path.suffix)
# # 测试提交pycharm


# import onnxruntime as ort
# sess = ort.InferenceSession(r"C:\Users\admin\Desktop\Defect-Detection\yolov5\runs\train\exp2\weights\best.onnx")
# for inp in sess.get_inputs():
#     print(f"Input: {inp.name}, shape: {inp.shape}")
# for out in sess.get_outputs():
#     print(f"Output: {out.name}, shape: {out.shape}")


# import onnx
#
# # 加载ONNX模型
# model_path = r"C:\Users\admin\Desktop\Defect-Detection\yolov5\runs\train\exp2\weights\best.onnx"  # 替换为你的模型路径
# model = onnx.load(model_path)
#
# # 打印模型输入信息
# print("=== 模型输入信息 ===")
# for input in model.graph.input:
#     name = input.name
#     shape = [dim.dim_value for dim in input.type.tensor_type.shape.dim]
#     print(f"输入名称: {name}")
#     print(f"输入形状: {shape}")
#     print("-" * 50)
#
# # 打印模型输出信息（核心！）
# print("\n=== 模型输出信息 ===")
# for output in model.graph.output:
#     name = output.name
#     shape = [dim.dim_value for dim in output.type.tensor_type.shape.dim]
#     print(f"输出名称: {name}")
#     print(f"输出形状: {shape}")
#     print(f"维度数: {len(shape)}")
#     print("-" * 50)
#
# # 验证是否符合YOLOv5 6.0标准
# print("\n=== 验证结果 ===")
# output_shapes = [
#     [dim.dim_value for dim in output.type.tensor_type.shape.dim]
#     for output in model.graph.output
# ]
#
# is_yolov5_60 = all(
#     len(shape) == 4 and shape[0] == 1 and shape[2] in [80,40,20] and shape[3] in [80,40,20]
#     for shape in output_shapes
# )
#
# if is_yolov5_60:
#     print("✅ 模型输出符合YOLOv5 6.0标准（4维张量）")
#     print("⚠️  你的C++代码中假设的5维[1,3,H,W,8]是错误的！")
# else:
#     print("❌ 模型输出不符合标准，请检查导出参数")


# import cv2
#
# model_path = r"C:\Users\admin\Desktop\Defect-Detection\yolov5\runs\train\exp2\weights\best.onnx"
# net = cv2.dnn.readNetFromONNX(model_path)
#
# # 获取所有输出层名称
# out_names = net.getUnconnectedOutLayersNames()
# print("输出层名称:", out_names)
#
# # 获取每个输出层的形状
# print("\n=== OpenCV读取的输出层形状 ===")
# for name in out_names:
#     layer_id = net.getLayerId(name)
#     layer = net.getLayer(layer_id)
#     # 获取输出形状（注意：OpenCV返回的是[channels, height, width]，batch维度在最前）
#     out_shape = net.getUnconnectedOutLayers()[layer_id-1].shape
#     print(f"输出层: {name}")
#     print(f"形状: {out_shape}")
#     print(f"维度数: {len(out_shape)}")
#     print("-" * 50)

# import onnx
# model = onnx.load(r"C:\Users\admin\Desktop\Defect-Detection\yolov5\runs\train\exp2\weights\yolov5s.onnx")
# for inp in model.graph.input:
#     print("Input:", inp.name, [d.dim_value for d in inp.type.tensor_type.shape.dim])
# for out in model.graph.output:
#     print("Output:", out.name, [d.dim_value for d in out.type.tensor_type.shape.dim])


# import cv2
# import sys
#
# model_path = r"C:\Users\admin\Desktop\Defect-Detection\yolov5\runs\train\exp2\weights\yolov5s.onnx"
# net = cv2.dnn.readNetFromONNX(model_path)
# print("OpenCV loaded ONNX successfully")


import os
from PIL import Image
from collections import defaultdict

def get_image_size_statistics(folder_path):
    # 支持的图片后缀
    img_suffix = (".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".webp")
    size_count = defaultdict(int)
    total_img = 0

    # 遍历文件夹
    for filename in os.listdir(folder_path):
        # 跳过文件夹，只处理文件
        file_path = os.path.join(folder_path, filename)
        if os.path.isdir(file_path):
            continue

        # 判断是否为图片
        if filename.lower().endswith(img_suffix):
            try:
                with Image.open(file_path) as img:
                    width, height = img.size
                    size_key = f"{width} × {height}"
                    size_count[size_key] += 1
                    total_img += 1
            except Exception as e:
                print(f"读取失败: {filename}，错误: {str(e)}")

    # 输出结果
    print("=" * 50)
    print(f"文件夹路径：{folder_path}")
    print(f"图片总数：{total_img} 张")
    print(f"不同尺寸总数：{len(size_count)} 种")
    print("=" * 50)
    print("尺寸明细（宽 × 高 | 数量）：")
    for size, cnt in sorted(size_count.items()):
        print(f"{size}  |  {cnt} 张")

if __name__ == "__main__":
    # ========== 在这里修改你的文件夹路径 ==========
    target_folder = r"C:\Users\admin\Desktop\Defect-Detection\yolov5\datasets\PCB\pcb_datasets"
    # ==============================================
    get_image_size_statistics(target_folder)