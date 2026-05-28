#ifndef IMAGE_READER_H
#define IMAGE_READER_H

#include <opencv2/opencv.hpp>

// 替代相机：读取本地图片 / 视频文件
// 接口和 CameraCapture 完全一致！
class ImageReader {
public:
    ImageReader();
    ~ImageReader();

    // 打开：传入图片路径 或 视频路径
    bool open(const std::string& file_path);

    // 获取帧（和相机用法一样）
    cv::Mat getFrame();

    // 关闭
    void close();

    // 是否打开成功
    bool isOpened();

private:
    cv::Mat src_image_;    // 单张图片
    cv::VideoCapture cap_; // 视频文件
    bool is_image_ = false;
    bool is_opened_ = false;
};

#endif