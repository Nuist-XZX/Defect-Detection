"""
Experimental modules
这个模块大多是在近年来的一些奇思妙想的论文中提出来的，yolov5的作者将这些模块与yolov5相结合，进行尝试。
"""

import numpy as np
import torch
import torch.nn as nn

from models.common import Conv
from utils.downloads import attempt_download


class CrossConv(nn.Module):
    # Cross Convolution Downsample
    def __init__(self, c1, c2, k=3, s=1, g=1, e=1.0, shortcut=False):
        # ch_in, ch_out, kernel, stride, groups, expansion, shortcut
        super().__init__()
        c_ = int(c2 * e)  # hidden channels
        self.cv1 = Conv(c1, c_, (1, k), (1, s))
        self.cv2 = Conv(c_, c2, (k, 1), (s, 1), g=g)
        self.add = shortcut and c1 == c2

    def forward(self, x):
        return x + self.cv2(self.cv1(x)) if self.add else self.cv2(self.cv1(x))


class Sum(nn.Module):
    # Weighted sum of 2 or more layers https://arxiv.org/abs/1911.09070
    def __init__(self, n, weight=False):  # n: number of inputs
        super().__init__()
        self.weight = weight  # apply weights boolean
        self.iter = range(n - 1)  # iter object
        if weight:
            self.w = nn.Parameter(-torch.arange(1., n) / 2, requires_grad=True)  # layer weights

    def forward(self, x):
        y = x[0]  # no weight
        if self.weight:
            w = torch.sigmoid(self.w) * 2
            for i in self.iter:
                y = y + x[i + 1] * w[i]
        else:
            for i in self.iter:
                y = y + x[i + 1]
        return y


class MixConv2d(nn.Module):
    # Mixed Depth-wise Conv https://arxiv.org/abs/1907.09595
    def __init__(self, c1, c2, k=(1, 3), s=1, equal_ch=True):
        super().__init__()
        groups = len(k)
        if equal_ch:  # equal c_ per group
            i = torch.linspace(0, groups - 1E-6, c2).floor()  # c2 indices
            c_ = [(i == g).sum() for g in range(groups)]  # intermediate channels
        else:  # equal weight.numel() per group
            b = [c2] + [0] * groups
            a = np.eye(groups + 1, groups, k=-1)
            a -= np.roll(a, 1, axis=1)
            a *= np.array(k) ** 2
            a[0] = 1
            c_ = np.linalg.lstsq(a, b, rcond=None)[0].round()  # solve for equal weight indices, ax = b

        self.m = nn.ModuleList([nn.Conv2d(c1, int(c_[g]), k[g], s, k[g] // 2, bias=False) for g in range(groups)])
        self.bn = nn.BatchNorm2d(c2)
        self.act = nn.LeakyReLU(0.1, inplace=True)

    def forward(self, x):
        return x + self.act(self.bn(torch.cat([m(x) for m in self.m], 1)))

# 如果模型推理时,用户给定多个权重,则会调用这个函数
class Ensemble(nn.ModuleList):
    # 模型集成：用来存放多个yolov5模型，把多个训练好的yolov5模型组合在一起推理
    def __init__(self):
        super().__init__()

    # 推理部分
    def forward(self, x, augment=False, profile=False, visualize=False):
        # x:输入图片 augment:数据增强 profile:性能分析 visualize:可视化
        y = []
        for module in self: # self相当于存放模型权重的列表,例如:[yolov5s.pt, yolov5m.pt, yolov5l.pt]
            # module(x, augment, profile, visualize)[0] 表示例如用模型yolov5s.pt去推理输入图片x,但模型会返回好几个东西,所以只取第一个[0]返回值,即是检测结果
            y.append(module(x, augment, profile, visualize)[0]) # 把每个模型的推理结果都存到列表y中保存
            # y = [模型1的输出,模型2的输出,模型3的输出,....],模型1的输出大概为[1,25200,85],主要是1张图,25200个预测框,85个参数(x,y,w,h,conf置信度,cls类别数量)
        # y = torch.stack(y).max(0)[0]  # 最大值融合: 取多个模型预测结果置信度最大的那个,过滤低置信度,但可能会丢失目标
        # y = torch.stack(y).mean(0)  # 平均值融合: 取多个模型预测结果的平均值,会更加平滑,但会拉低高置信度预测,可能会出现目标重叠
        y = torch.cat(y, 1)  # NMS集成,把所有模型的推理结果直接拼接在一起,后续统一做一次NMS去重。将y按模型输出的第1个维度,即预测框数量维度进行拼接,得到结果大概为[1,75600,85] - 这里假设的是3个模型,每个模型有25200个预测框
        return y, None  # inference, train output

# 普通模型加载 .pt  这个函数用于加载模型权重文件并构建模型（可以构造普通模型或者集成模型）
def attempt_load(weights, map_location=None, inplace=True, fuse=True):
    from models.yolo import Detect, Model
    """
        这个函数用于加载模型权重文件并构建模型（可以构造普通模型或者集成模型）。
                用在val.py、detect.py、train.py等文件中  一般用在测试、验证阶段
        Loads an ensemble of models weights=[a,b,c] or a single model weights=[a] or weights=a
        :params weights: 模型的权重文件地址 默认weights/yolov5s.pt
                         可以是[a]也可以是list格式[a, b]  如果是list格式将调用上面的模型集成函数Ensemble 多模型运算 提高最终模型的泛化误差
        :params map_location: 表示模型运行设备device
        :params inplace: pytorch 1.7.0 compatibility设置
    """
    # model = Ensemble() 创建一个实例对象,把这个对象赋值给model
    model = Ensemble() # 创建一个空的模型容器,不会加载任何权重,里面没有任何模型,此时len(model) = 0;
    for w in weights if isinstance(weights, list) else [weights]:
        ckpt = torch.load(attempt_download(w), map_location=map_location)  # 判断是否存在模型权重文件,不存在则下载,存在则转为路径,并且加载权重文件
        """
            ckpt变成一个字典,包含下列内容:
            ckpt = {
                "model": 模型结构与权重,
                "optimizer": 优化器参数,
                "epoch": 300,
                "cfg": 配置文件,
                ...
            }
        """
        if fuse:
            # 从加载好的文件里,拿出最好的模型 → 转成浮点型 → 模型融合提速 → 设为推理模式 → 把模型放进集成容器里,即把所有权重模型传参到Ensemble中
            # ckpt['ema' if ckpt.get('ema') else 'model'] 选最好、最稳定的模型,ckpt.get('ema'):判断权重文件是不是 EMA 模型,EMA是滑动平均模型,效果比普通模型更好更稳定
            # .float() 转为32位浮点型 .fuse() 融合Conv和BN层,提升推理速度 .eval() 告诉模型“现在是预测,不是训练”,关闭dropout、BatchNorm等训练时用的层,提升推理速度
            model.append(ckpt['ema' if ckpt.get('ema') else 'model'].float().fuse().eval())  # FP32 model
            # 类似于model.append(yolov5)...  model中存放的就是yolov5的模型结构和权重
        else:
            model.append(ckpt['ema' if ckpt.get('ema') else 'model'].float().eval())  # without layer fuse


    # Compatibility updates 兼容性更新
    for m in model.modules(): # 遍历模型里的所有层(卷积、BN、激活函数…… 从头到尾,一个不漏)
        if type(m) in [nn.Hardswish, nn.LeakyReLU, nn.ReLU, nn.ReLU6, nn.SiLU, Detect, Model]:
            # 针对激活函数层,设置inplace属性为True,提升推理速度
            m.inplace = inplace  # pytorch 1.7.0 compatibility
            if type(m) is Detect: # 如果是检测层
                if not isinstance(m.anchor_grid, list):  # 如果 anchor_grid 不是列表
                    delattr(m, 'anchor_grid') # 删除旧版的anchor_grid
                    setattr(m, 'anchor_grid', [torch.zeros(1)] * m.nl) # 新建新版格式
                    """
                        anchor_grid = [
                            # 第1个检测头 80x80特征图
                            shape: [1, 3, 80, 80, 2]
                            内容: 每个格子上3个anchor的 (w, h)
                        
                            # 第2个检测头 40x40
                            shape: [1, 3, 40, 40, 2]
                        
                            # 第3个检测头 20x20
                            shape: [1, 3, 20, 20, 2]
                        ]
                        最终预测框 = 网格坐标 + 模型预测的偏移量 × anchor_grid里的宽高
                    """

        elif type(m) is Conv:
            m._non_persistent_buffers_set = set()  # pytorch 1.6.0 compatibility

    if len(model) == 1: # 如果只有一个模型,直接返回model
        return model[-1]  # return model
    else:
        # 如果是多个模型
        print(f'Ensemble created with {weights}\n')
        for k in ['names']:
            setattr(model, k, getattr(model[-1], k))
        model.stride = model[torch.argmax(torch.tensor([m.stride.max() for m in model])).int()].stride  # max stride
        return model  # return ensemble
