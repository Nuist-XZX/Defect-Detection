#include "PreProcessor.h"
#include <cmath>

PreProcessor::PreProcessor(int stride) : stride_(stride) {}

cv::Mat PreProcessor::letterbox(const cv::Mat& src, int target_width, int target_height) {
    int src_w = src.cols;
    int src_h = src.rows;

    float r = std::min((float)target_width / src_w, (float)target_height / src_h);
    scale_ = r;

    int new_w = std::round(src_w * r);
    int new_h = std::round(src_h * r);

    int dw = target_width - new_w;
    int dh = target_height - new_h;

    // stride 对齐（auto=True）
    dw = dw % stride_;
    dh = dh % stride_;

    int pad_left = dw / 2;
    int pad_top = dh / 2;

    pad_left_ = pad_left;
    pad_top_ = pad_top;

    cv::Mat resized;
    cv::resize(src, resized, cv::Size(new_w, new_h), 0, 0, cv::INTER_LINEAR);

    cv::Mat dst;
    cv::copyMakeBorder(resized, dst,
        pad_top, dh - pad_top,
        pad_left, dw - pad_left,
        cv::BORDER_CONSTANT,
        cv::Scalar(114, 114, 114));
    return dst;
}

void PreProcessor::getScaleAndPad(float& scale, int& pad_left, int& pad_top) {
    scale = scale_;
    pad_left = pad_left_;
    pad_top = pad_top_;
}