#include "YoloV5Detector.h"
#include <iostream>
#include <cmath>

static inline float sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

bool YoloV5Detector::loadModel(const std::string& onnx_path) {
    try {
        net_ = cv::dnn::readNetFromONNX(onnx_path);
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "模型加载失败: " << e.what() << std::endl;
        return false;
    }
}

void YoloV5Detector::decodeLayer(const cv::Mat& output, int stride, const std::vector<float>& anchors,
    float scale, int pad_left, int pad_top, const cv::Size& src_size,
    std::vector<DetectResult>& results) {
    // output shape: [1, 3, H, W, 8]
    int H = output.size[2];
    int W = output.size[3];
    float* data = (float*)output.data;
    int anchor_cnt = 3;

    for (int a = 0; a < anchor_cnt; ++a) {
        float anchor_w = anchors[a * 2];
        float anchor_h = anchors[a * 2 + 1];
        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < W; ++j) {
                int offset = a * H * W * 8 + i * W * 8 + j * 8;
                float* ptr = data + offset;

                float obj_conf = sigmoid(ptr[4]);
                if (obj_conf < conf_thresh_) continue;

                float max_cls = 0;
                int cls_id = 0;
                for (int c = 5; c < 5 + num_class_; ++c) {
                    float cls_score = sigmoid(ptr[c]);
                    if (cls_score > max_cls) {
                        max_cls = cls_score;
                        cls_id = c - 5;
                    }
                }
                float final_conf = obj_conf * max_cls;
                if (final_conf < conf_thresh_) continue;

                float cx = (j + sigmoid(ptr[0])) * stride;
                float cy = (i + sigmoid(ptr[1])) * stride;
                float w = std::exp(ptr[2]) * anchor_w;
                float h = std::exp(ptr[3]) * anchor_h;

                // 映射回原始图像（与 Python scale_coords 等价）
                int x1 = (int)((cx - w / 2 - pad_left) / scale);
                int y1 = (int)((cy - h / 2 - pad_top) / scale);
                int x2 = (int)((cx + w / 2 - pad_left) / scale);
                int y2 = (int)((cy + h / 2 - pad_top) / scale);

                x1 = std::max(0, x1);
                y1 = std::max(0, y1);
                x2 = std::min(src_size.width, x2);
                y2 = std::min(src_size.height, y2);
                if (x2 <= x1 || y2 <= y1) continue;

                results.push_back({ cv::Rect(x1, y1, x2 - x1, y2 - y1), cls_id, final_conf });
            }
        }
    }
}

std::vector<DetectResult> YoloV5Detector::detect(cv::Mat& src, PreProcessor& pre) {
    const int input_w = 640, input_h = 640;
    cv::Mat blob_img = pre.letterbox(src, input_w, input_h);
    cv::Mat blob;
    cv::dnn::blobFromImage(blob_img, blob, 1 / 255.0, cv::Size(input_w, input_h), cv::Scalar(), true, false);
    net_.setInput(blob);

    // 获取所有输出层
    std::vector<cv::String> outNames = net_.getUnconnectedOutLayersNames();
    std::vector<cv::Mat> outputs;
    net_.forward(outputs, outNames);

    // 自动识别三个特征层（根据形状 [1,3,H,W,8] 和 H 判断 stride）
    std::vector<cv::Mat> layerOutputs;
    std::vector<int> layerStrides;
    for (const auto& out : outputs) {
        if (out.dims == 5 && out.size[1] == 3 && out.size[4] == 8) {
            int H = out.size[2];
            if (H == 80) layerStrides.push_back(8);
            else if (H == 40) layerStrides.push_back(16);
            else if (H == 20) layerStrides.push_back(32);
            else continue;
            layerOutputs.push_back(out);
        }
    }

    if (layerOutputs.empty()) {
        std::cerr << "错误：未找到三个特征层输出，请检查 ONNX 模型。" << std::endl;
        return {};
    }

    float scale;
    int pad_left, pad_top;
    pre.getScaleAndPad(scale, pad_left, pad_top);

    std::vector<DetectResult> results;
    for (size_t i = 0; i < layerOutputs.size(); ++i) {
        int stride = layerStrides[i];
        const std::vector<float>* anchor_ptr = nullptr;
        if (stride == 8) anchor_ptr = &anchors_[0];
        else if (stride == 16) anchor_ptr = &anchors_[1];
        else if (stride == 32) anchor_ptr = &anchors_[2];
        if (anchor_ptr) {
            decodeLayer(layerOutputs[i], stride, *anchor_ptr, scale, pad_left, pad_top, src.size(), results);
        }
    }

    std::cout << "Before NMS: " << results.size() << " boxes" << std::endl;
    nms(results);
    std::cout << "After NMS: " << results.size() << " boxes" << std::endl;
    return results;
}

void YoloV5Detector::nms(std::vector<DetectResult>& results) {
    if (results.empty()) return;
    std::vector<cv::Rect> boxes;
    std::vector<float> confs;
    for (const auto& r : results) {
        boxes.push_back(r.box);
        confs.push_back(r.confidence);
    }
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confs, conf_thresh_, iou_thresh_, indices);
    std::vector<DetectResult> nms_res;
    for (int idx : indices) {
        nms_res.push_back(results[idx]);
    }
    results = nms_res;
}