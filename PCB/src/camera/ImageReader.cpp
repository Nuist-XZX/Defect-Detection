#include "ImageReader.h"
#include <iostream>

ImageReader::ImageReader() {}

ImageReader::~ImageReader() {
    close();
}

// 打开图片或视频
bool ImageReader::open(const std::string& file_path) {
    // 先尝试当作图片读取
    src_image_ = cv::imread(file_path);
    if (!src_image_.empty()) {
        is_image_ = true;
        is_opened_ = true;
        std::cout << "[Info] 读取图片成功: " << file_path << std::endl;
        return true;
    }

    // 尝试当作视频读取
    if (cap_.open(file_path)) {
        is_image_ = false;
        is_opened_ = true;
        std::cout << "[Info] 打开视频成功: " << file_path << std::endl;
        return true;
    }

    std::cerr << "[Error] 打开失败: " << file_path << std::endl;
    return false;
}

// 获取帧（对外接口和相机完全一致）
cv::Mat ImageReader::getFrame() {
    if (is_image_) {
        return src_image_.clone(); // 图片一直返回这张
    } else {
        cv::Mat frame;
        cap_ >> frame;
        return frame;
    }
}

void ImageReader::close() {
    if (cap_.isOpened()) {
        cap_.release();
    }
    is_opened_ = false;
}

bool ImageReader::isOpened() {
    return is_opened_;
}