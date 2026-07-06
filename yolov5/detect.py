import argparse
import os
import sys
from pathlib import Path

import cv2
import numpy as np
import torch
import torch.backends.cudnn as cudnn

FILE = Path(__file__).resolve()
ROOT = FILE.parents[0]  # YOLOv5 root directory
if str(ROOT) not in sys.path:
    sys.path.append(str(ROOT))  # add ROOT to PATH
ROOT = Path(os.path.relpath(ROOT, Path.cwd()))  # relative

from models.experimental import attempt_load
from utils.datasets import LoadImages, LoadStreams
from utils.general import apply_classifier, check_img_size, check_imshow, check_requirements, check_suffix, colorstr, \
    increment_path, non_max_suppression, print_args, save_one_box, scale_coords, set_logging, \
    strip_optimizer, xyxy2xywh
from utils.plots import Annotator, colors
from utils.torch_utils import load_classifier, select_device, time_sync


@torch.no_grad()
def run(weights=ROOT / 'yolov5s.pt',  # model.pt path(s)
        source=ROOT / 'data/images',  # file/dir/URL/glob, 0 for webcam
        imgsz=640,  # inference size (pixels)
        conf_thres=0.25,  # confidence threshold 用在NMS中
        iou_thres=0.45,  # NMS IOU threshold 用在NMS中
        max_det=1000,  # 每张图片上检测的最大目标数量，用在NMS中
        device='',  # cuda device, i.e. 0 or 0,1,2,3 or cpu
        view_img=False,  # show results
        save_txt=False,  # save results to *.txt
        save_conf=False,  # save confidences in --save-txt labels
        save_crop=False,  # save cropped prediction boxes
        nosave=False,  # do not save images/videos 是否不保存检测结果的的图像或视频
        classes=None,  # filter by class: --class 0, or --class 0 2 3 按类别过滤,形如0或者0 2 3
        agnostic_nms=False,  # class-agnostic NMS 进行nms是否也去除不同类别之间的框，默认False
        augment=False,  # augmented inference 推理时是否使用数据增强
        visualize=False,  # visualize features
        update=False,  # update all models
        project=ROOT / 'runs/detect',  # save results to project/name
        name='exp',  # save results to project/name
        exist_ok=False,  # existing project/name ok, do not increment
        line_thickness=3,  # bounding box thickness (pixels)
        hide_labels=False,  # hide labels
        hide_conf=False,  # hide confidences
        half=False,  # use FP16 half-precision inference 是否用半精度推理(节约显存)
        dnn=False,  # use OpenCV DNN for ONNX inference 是否用OpenCV DNN 预测作为ONNX推理的后端
        ):

    source = str(source)
    save_img = not nosave and not source.endswith('.txt')  # 保存推理图片 True
    '''
        source.lower().startswith()判断source是否是链接 网络视频流地址
        source.endswith('.txt')判断source是否是txt文件路径 即存放视频流的地址
        source.isnumeric()判断source是否是数字组成 即摄像头编号
        判断source是否是 摄像头 / 视频流
    '''
    webcam = source.isnumeric() or source.endswith('.txt') or source.lower().startswith(('rtsp://', 'rtmp://', 'http://', 'https://'))

    # 保存目录
    save_dir = increment_path(Path(project) / name, exist_ok=exist_ok) # 更新保存目录序号exp exp2 exp3...
    (save_dir / 'labels' if save_txt else save_dir).mkdir(parents=True, exist_ok=True)  # # 创建保存目录C:\Users\admin\Desktop\yolov5-6.0\runs\detect\exp2

    # 初始化
    set_logging()
    device = select_device(device)
    half &= device.type != 'cpu'  # 半精度只允许在gpu上运行 CUDA

    # 加载模型
    w = str(weights[0] if isinstance(weights, list) else weights) # 权重文件路径，如果是列表，就取列表的第一个权重路径
    classify, suffix, suffixes = False, Path(w).suffix.lower(), ['.pt', '.onnx', '.tflite', '.pb', '']
    check_suffix(w, suffixes)  # 检查权重文件格式是否符合要求
    pt, onnx, tflite, pb, saved_model = (suffix == x for x in suffixes)  # 一次性判断权重文件是哪种格式，分别赋值给pt、onnx、tflite、pb、saved_model，是的赋值True，否则赋值False
    stride, names = 64, [f'class{i}' for i in range(1000)]  # stride是步长，names是类别名称（这边设置的是默认值）
    if pt:
        # 判断权重路径中是否包含torchscript关键词，TorchScript是Pytorch的一种序列化模型格式。如果没有则使用普通的torch模型.pt格式
        # model是由Ensemble实例的对象,其forward用于推理, return y, None  # inference, train output
        model = torch.jit.load(w) if 'torchscript' in w else attempt_load(weights, map_location=device) # 模型加载
        stride = int(model.stride.max())  # model stride 拿到模型最大下采样倍数stride[8,16,32],与anchor框倍数相同
        names = model.module.names if hasattr(model, 'module') else model.names  # 拿到模型里的类别名称,即数据集标签中的类别名称
        if half: # 是否开启半精度推理,将模型从Float32转为Float16
            model.half()  # to FP16 速度更快,显存减半,gpu上推理开,cpu不支持
        if classify:  # 二级分类器,检测后是否使用一个专门的分类模型resnet50二次确认物体类别,因为yolo检测出的分类精度不如分类网络,但是这边是不使用
            # YOLO 检测出框 → 把框里的图抠出来 → ResNet50 再精分类
            modelc = load_classifier(name='resnet50', n=2)  # initialize
            modelc.load_state_dict(torch.load('resnet50.pt', map_location=device)['model']).to(device).eval()
    elif onnx:
        if dnn:
            # 如果使用OpenCV DNN作为后端
            # check_requirements(('opencv-python>=4.5.4',))
            net = cv2.dnn.readNetFromONNX(w) # 用 OpenCV 直接读取并运行 ONNX 模型
            # 不用装额外的依赖,c++、python都能用,部署方便
        else:
            # 如果不用DNN,而是用微软官方的 onnxruntime 运行 ONNX 模型
            check_requirements(('onnx', 'onnxruntime'))
            import onnxruntime
            session = onnxruntime.InferenceSession(w, None)
            # 速度更快,支持gpu,兼容性强
    else:  # TensorFlow models
        check_requirements(('tensorflow>=2.4.1',))
        import tensorflow as tf
        if pb:  # https://www.tensorflow.org/guide/migrate#a_graphpb_or_graphpbtxt
            def wrap_frozen_graph(gd, inputs, outputs):
                x = tf.compat.v1.wrap_function(lambda: tf.compat.v1.import_graph_def(gd, name=""), [])  # wrapped import
                return x.prune(tf.nest.map_structure(x.graph.as_graph_element, inputs),
                               tf.nest.map_structure(x.graph.as_graph_element, outputs))

            graph_def = tf.Graph().as_graph_def()
            graph_def.ParseFromString(open(w, 'rb').read())
            frozen_func = wrap_frozen_graph(gd=graph_def, inputs="x:0", outputs="Identity:0")
        elif saved_model:
            model = tf.keras.models.load_model(w)
        elif tflite:
            interpreter = tf.lite.Interpreter(model_path=w)  # load TFLite model
            interpreter.allocate_tensors()  # allocate
            input_details = interpreter.get_input_details()  # inputs
            output_details = interpreter.get_output_details()  # outputs
            int8 = input_details[0]['dtype'] == np.uint8  # is TFLite quantized uint8 model
    imgsz = check_img_size(imgsz, s=stride)  # (如果是pt)检查并修改定义的输入图片的尺寸(即640或自定义的其他尺寸)为yolov5最大下采样倍数32的倍数,(如果是onnx)下采样倍数是默认值64

    # 数据加载 推理前数据预处理
    if webcam:
        # 如果推理数据是视频流/摄像头
        view_img = check_imshow() # 检查当前环境是否支持OpenCV 的 imshow 显示
        cudnn.benchmark = True  # 设置 True 来加快固定尺寸图片的推理速度  如果尺寸一直在变会影响推理速度,占用更多显存
        # LoadStreams return self.sources, img, img0, None
        dataset = LoadStreams(source, img_size=imgsz, stride=stride, auto=pt)
        bs = len(dataset)  # batch_size  即多少路视频流/摄像头
    else:
        # 如果推理数据是图片/视频
        # LoadImages path, img, img0, self.cap
        dataset = LoadImages(source, img_size=imgsz, stride=stride, auto=pt) # 加载图片/视频并预处理
        bs = 1  # batch_size
    vid_path, vid_writer = [None] * bs, [None] * bs #  视频保存路径，视频写入器

    # 开始推理
    if pt and device.type != 'cpu':
        # 如果在gpu运行并且是pt模型,torch.zeros(1, 3, *imgsz)是为了初始化模型,使其可以运行,因为pt模型需要输入尺寸才能初始化
        # 这里的1是一张图片,3是通道数,imgsz是输入图片的尺寸
        # type_as(next(model.parameters())) 让数据类型和模型权重完全一致（半精度 / 全精度）
        model(torch.zeros(1, 3, *imgsz).to(device).type_as(next(model.parameters())))  # run once 只运行一次,目的是预热模型
    """
        dt 是 detect time 的缩写，意思是检测耗时
        它是一个包含 3 个浮点数的列表，用来记录三段关键耗时：
            dt[0]：图片预处理时间（缩放、填充、转张量等）
            dt[1]：模型推理时间（神经网络前向计算）
            dt[2]：后处理时间（NMS 非极大值抑制、画框、输出结果）
        作用：统计并累加每一步的耗时，最后打印出速度信息，例如：
            速度：预处理 0.5ms，推理 10.2ms，后处理 0.8ms
        
        seen：表示已经处理过的图片 / 视频帧总数, 梅=每处理一帧就加 1
    """
    dt, seen = [0.0, 0.0, 0.0], 0
    for path, img, im0s, vid_cap in dataset:
        t1 = time_sync() # 计时开始
        if onnx:
            # onnx 模型转Float32
            img = img.astype('float32')
        else:
            # pt模型 → 转 torch 张量 → 送到 GPU → 转半精度 / 全精度
            img = torch.from_numpy(img).to(device)
            img = img.half() if half else img.float()  # uint8 to fp16/32
        img = img / 255.0  # 归一化,只有归一化才能正常推理 0 - 255 to 0.0 - 1.0
        if len(img.shape) == 3:
            # img[None]等价于img.unsqueeze(0),即在第 0 号位置（最前面）插入Batch维度。例如(3, 640, 640) → unsqueeze(0) → (1, 3, 640, 640)
            img = img[None]  # 扩展Batch维度,使img变成[B,C,H,W]
        t2 = time_sync() # 预处理计时结束
        dt[0] += t2 - t1 # 记录预处理时间

        # Inference
        if pt:
            # 是否开启特征图可视化,开启则创建保存文件夹,普通检测不用管,因为pt模型的特征图是可视化的
            visualize = increment_path(save_dir / Path(path).stem, mkdir=True) if visualize else False
            # 推理 augment=True 开启数据增强,visualize=True 开启特征图可视化
            pred = model(img, augment=augment, visualize=visualize)[0]
            # pred.shape = [1, 25200, 85] 1张图片,25200是预测框的数量((80² + 40² + 20²) × 3 = 25200),85是类别数、坐标(x,y,w,h)、置信度等
        elif onnx:
            if dnn:
                # 如果使用DNN,则使用Opencv读取onnx的模型
                net.setInput(img) # 给模型设置输入
                pred = torch.tensor(net.forward()) # 推理 → 转成张量
            else:
                # 使用官方ONNX Runtime
                pred = torch.tensor(session.run([session.get_outputs()[0].name], {session.get_inputs()[0].name: img}))
        else:  # tensorflow model (tflite, pb, saved_model)
            imn = img.permute(0, 2, 3, 1).cpu().numpy()  # image in numpy
            if pb:
                pred = frozen_func(x=tf.constant(imn)).numpy()
            elif saved_model:
                pred = model(imn, training=False).numpy()
            elif tflite:
                if int8:
                    scale, zero_point = input_details[0]['quantization']
                    imn = (imn / scale + zero_point).astype(np.uint8)  # de-scale
                interpreter.set_tensor(input_details[0]['index'], imn)
                interpreter.invoke()
                pred = interpreter.get_tensor(output_details[0]['index'])
                if int8:
                    scale, zero_point = output_details[0]['quantization']
                    pred = (pred.astype(np.float32) - zero_point) * scale  # re-scale
            pred[..., 0] *= imgsz[1]  # x
            pred[..., 1] *= imgsz[0]  # y
            pred[..., 2] *= imgsz[1]  # w
            pred[..., 3] *= imgsz[0]  # h
            pred = torch.tensor(pred)
        t3 = time_sync() # 推理计时结束
        dt[1] += t3 - t2 # 记录推理时间

        # 后处理部分: 包括NMS(类别筛选、置信度筛选)、scale_coords还原缩放坐标、xyxy->xywh、画图、检测框边界截断
        # NMS
        # 原pred pred.shape = [1, 25200, 85], 新pred 是个列表
        # pred = [
        #     tensor([[x1,y1,x2,y2,conf,cls],  # 图1的框1
        #             [x1,y1,x2,y2,conf,cls],  # 图1的框2
        #             ... ]),
        #
        #     tensor([[x1,y1,x2,y2,conf,cls],  # 图2的框1
        #             ... ])
        # ]
        pred = non_max_suppression(pred, conf_thres, iou_thres, classes, agnostic_nms, max_det=max_det)
        # print('pred:', pred)
        dt[2] += time_sync() - t3 # 后处理时间

        # Second-stage classifier (optional) 是否使用二级分类器,让分类更准确
        if classify:
            pred = apply_classifier(pred, modelc, img, im0s)

        # Process predictions
        for i, det in enumerate(pred):  # i = 第几张图, det = 每张图的所有检测框
            seen += 1
            if webcam:  # batch_size >= 1
                p, s, im0, frame = path[i], f'{i}: ', im0s[i].copy(), dataset.count
            else:
                p, s, im0, frame = path, '', im0s.copy(), getattr(dataset, 'frame', 0)

            p = Path(p)  # to Path
            save_path = str(save_dir / p.name)  # img.jpg 如:runs/detect/exp/bus.jpg
            txt_path = str(save_dir / 'labels' / p.stem) + ('' if dataset.mode == 'image' else f'_{frame}')  # img.txt
            s += '%gx%g ' % img.shape[2:]  # print string 640*640
            gn = torch.tensor(im0.shape)[[1, 0, 1, 0]]  # 原图的宽度高度
            imc = im0.copy() if save_crop else im0  # for save_crop
            annotator = Annotator(im0, line_width=line_thickness, example=str(names))
            if len(det):
                # 如果检测框数量大于0,开始画框
                # Rescale boxes from img_size to im0 size
                # 将框从 img_size 缩放到 im0 大小
                det[:, :4] = scale_coords(img.shape[2:], det[:, :4], im0.shape).round()

                # Print results
                for c in det[:, -1].unique():
                    n = (det[:, -1] == c).sum()  # detections per class
                    s += f"{n} {names[int(c)]}{'s' * (n > 1)}, "  # add to string

                # Write results
                for *xyxy, conf, cls in reversed(det):
                    if save_txt:  # Write to file
                        # xyxy2xywh(torch.tensor(xyxy).view(1, 4))得到中心点坐标和长宽后,使用gn来归一化
                        xywh = (xyxy2xywh(torch.tensor(xyxy).view(1, 4)) / gn).view(-1).tolist()  # normalized xywh
                        line = (cls, *xywh, conf) if save_conf else (cls, *xywh)  # label format
                        with open(txt_path + '.txt', 'a') as f:
                            f.write(('%g ' * len(line)).rstrip() % line + '\n')

                    if save_img or save_crop or view_img:  # Add bbox to image
                        c = int(cls)  # integer class
                        label = None if hide_labels else (names[c] if hide_conf else f'{names[c]} {conf:.2f}')
                        annotator.box_label(xyxy, label, color=colors(c, True))
                        if save_crop:
                            # 保存检测框
                            save_one_box(xyxy, imc, file=save_dir / 'crops' / names[c] / f'{p.stem}.jpg', BGR=True)

            # Print time (inference-only)
            print(f'{s}Done. ({t3 - t2:.3f}s)')

            # Stream results
            im0 = annotator.result() # 把所有画好框、写好字的图片取出来
            # 弹窗显示
            if view_img:
                cv2.imshow(str(p), im0)
                cv2.waitKey(1)  # 1 millisecond

            # 保存推理图片
            if save_img:
                if dataset.mode == 'image':
                    cv2.imwrite(save_path, im0)
                else:  # 'video' or 'stream'
                    if vid_path[i] != save_path:  # new video
                        vid_path[i] = save_path
                        if isinstance(vid_writer[i], cv2.VideoWriter):
                            vid_writer[i].release()  # release previous video writer
                        if vid_cap:  # video
                            fps = vid_cap.get(cv2.CAP_PROP_FPS)
                            w = int(vid_cap.get(cv2.CAP_PROP_FRAME_WIDTH))
                            h = int(vid_cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
                        else:  # stream
                            fps, w, h = 30, im0.shape[1], im0.shape[0]
                            save_path += '.mp4'
                        vid_writer[i] = cv2.VideoWriter(save_path, cv2.VideoWriter_fourcc(*'mp4v'), fps, (w, h))
                    vid_writer[i].write(im0)

    # Print results
    t = tuple(x / seen * 1E3 for x in dt)  # speeds per image
    print(f'Speed: %.1fms pre-process, %.1fms inference, %.1fms NMS per image at shape {(1, 3, *imgsz)}' % t)
    if save_txt or save_img:
        s = f"\n{len(list(save_dir.glob('labels/*.txt')))} labels saved to {save_dir / 'labels'}" if save_txt else ''
        print(f"Results saved to {colorstr('bold', save_dir)}{s}")
    if update:
        strip_optimizer(weights)  # update model (to fix SourceChangeWarning)


def parse_opt():
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, default=ROOT / 'runs/train/exp2/weights/best.onnx', help='model path(s)')
    parser.add_argument('--source', type=str, default=ROOT / 'data/images', help='file/dir/URL/glob, 0 for webcam')
    parser.add_argument('--imgsz', '--img', '--img-size', nargs='+', type=int, default=[640], help='inference size h,w')
    parser.add_argument('--conf-thres', type=float, default=0.25, help='confidence threshold')
    parser.add_argument('--iou-thres', type=float, default=0.45, help='NMS IoU threshold')
    parser.add_argument('--max-det', type=int, default=1000, help='maximum detections per image')
    parser.add_argument('--device', default='', help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    parser.add_argument('--view-img', action='store_true', help='show results')
    parser.add_argument('--save-txt', action='store_true', help='save results to *.txt')
    parser.add_argument('--save-conf', action='store_true', help='save confidences in --save-txt labels')
    parser.add_argument('--save-crop', action='store_true', help='save cropped prediction boxes')
    parser.add_argument('--nosave', action='store_true', help='do not save images/videos')
    parser.add_argument('--classes', nargs='+', type=int, help='filter by class: --classes 0, or --classes 0 2 3')
    parser.add_argument('--agnostic-nms', action='store_true', help='class-agnostic NMS')
    parser.add_argument('--augment', action='store_true', help='augmented inference')
    parser.add_argument('--visualize', action='store_true', help='visualize features')
    parser.add_argument('--update', action='store_true', help='update all models')
    parser.add_argument('--project', default=ROOT / 'runs/detect', help='save results to project/name')
    parser.add_argument('--name', default='exp', help='save results to project/name')
    parser.add_argument('--exist-ok', action='store_true', help='existing project/name ok, do not increment')
    parser.add_argument('--line-thickness', default=3, type=int, help='bounding box thickness (pixels)')
    parser.add_argument('--hide-labels', default=False, action='store_true', help='hide labels')
    parser.add_argument('--hide-conf', default=False, action='store_true', help='hide confidences')
    parser.add_argument('--half', action='store_true', help='use FP16 half-precision inference')
    parser.add_argument('--dnn', action='store_true', help='use OpenCV DNN for ONNX inference')
    opt = parser.parse_args()
    opt.imgsz *= 2 if len(opt.imgsz) == 1 else 1  # expand
    print_args(FILE.stem, opt)
    return opt


def main(opt):
    check_requirements(exclude=('tensorboard', 'thop')) # 检查程序所需的依赖项是否已安装
    run(**vars(opt)) # 将opt变量属性和属性值作为关键字参数传递给run()函数


if __name__ == "__main__":
    opt = parse_opt()
    main(opt)
