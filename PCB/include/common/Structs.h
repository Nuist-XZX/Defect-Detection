#ifndef STRUCTS_H
#define STRUCTS_H

#include <opencv2/opencv.hpp>

struct DetectResult {
    cv::Rect box;
    int class_id;
    float confidence;
};

#endif