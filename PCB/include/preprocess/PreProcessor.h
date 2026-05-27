#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

#include <opencv2/opencv.hpp>

class PreProcessor {
public:
    PreProcessor(int stride = 32);
    ~PreProcessor() = default;

    cv::Mat letterbox(const cv::Mat& src, int target_width, int target_height);
    void getScaleAndPad(float& scale, int& pad_left, int& pad_top);

private:
    float scale_ = 1.0f;
    int pad_left_ = 0;
    int pad_top_ = 0;
    int stride_ = 32;
};

#endif