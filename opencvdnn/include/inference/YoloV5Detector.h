#ifndef YOLOV5_DETECTOR_H
#define YOLOV5_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "Structs.h"
#include "PreProcessor.h"

class YoloV5Detector {
public:
    YoloV5Detector() = default;
    ~YoloV5Detector() = default;

    bool loadModel(const std::string& onnx_path);
    std::vector<DetectResult> detect(cv::Mat& src, PreProcessor& pre);

private:
    void nms(std::vector<DetectResult>& results);
    void decodeLayer(const cv::Mat& output, int stride, const std::vector<float>& anchors,
        float scale, int pad_left, int pad_top, const cv::Size& src_size,
        std::vector<DetectResult>& results);

    cv::dnn::Net net_;
    const float conf_thresh_ = 0.25f;
    const float iou_thresh_ = 0.45f;
    const int num_class_ = 3;   // ⚠️ 改成你的类别数

    // YOLOv5 默认 anchors（顺序 stride8,16,32）
    const std::vector<std::vector<float>> anchors_ = {
        {10,13, 16,30, 33,23},
        {30,61, 62,45, 59,119},
        {116,90, 156,198, 373,326}
    };
    const std::vector<int> strides_ = { 8, 16, 32 };
};

#endif