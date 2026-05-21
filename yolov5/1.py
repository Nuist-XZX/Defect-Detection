# import os
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
#     img_ext: str = ".png",
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
#     IMAGES_DIR = r"C:\Users\admin\Desktop\470\ug470_7Series_Config"    # PNG图片文件夹路径
#     LABELS_DIR = r"C:\Users\admin\Desktop\470\labels"        # 标签文件夹路径
#     OUTPUT_ROOT = r"C:\Users\admin\Desktop\470\yolov"    # 输出根目录
#     # ======================================
#
#     split_dataset(
#         images_dir=IMAGES_DIR,
#         labels_dir=LABELS_DIR,
#         output_root=OUTPUT_ROOT,
#         train_ratio=0.8,
#         val_ratio=0.1,
#         test_ratio=0.1,
#         img_ext=".png",    # 如果图片不是 .png 可修改
#         label_ext=".txt",  # 如果标签不是 .txt 可修改（例如 .png 或 .json）
#         random_seed=42
#     )


from pathlib import Path

path1 = r'C:\Users\admin\Desktop\470\labels'
path = Path(path1)
print(path)
print(path.suffix)








