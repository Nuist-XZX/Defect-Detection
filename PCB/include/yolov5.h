#pragma once
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <utility>
#include <codecvt>
#include <fstream>

//输出结构体：存储单个检测结果
struct Output {
    cv::Rect box;		// OpenCV矩形(x,y,w,h)
    float conf{};		// C++11值初始化：等价于conf=0.0f（置信度）
    int class_index{};	// 等价于class_index=0（类别索引）
};


class YOLODetector
{
public:
    explicit YOLODetector(std::nullptr_t) {};
	
    YOLODetector(const std::string& modelPath, const bool& isGPU, const cv::Size& inputSize);
    
	std::vector<Output> detect(cv::Mat& image, const float& confThreshold, const float& iouThreshold);
    
	void draw(cv::Mat& image, std::vector<Output>& detections, const std::vector<std::string>& classNames);

private:
    Ort::Env env{ nullptr };
    Ort::SessionOptions sessionOptions{ nullptr };
    Ort::Session session{ nullptr };

    void preprocessing(cv::Mat& image, float*& blob, cv::Vec4d& params, std::vector<int64_t>& inputTensorShape);
    
	std::vector<Output> postprocessing(const cv::Size& resizedImageShape,
        const cv::Size& originalImageShape,
        std::vector<Ort::Value>& outputTensors,
        const float& confThreshold, const float& iouThreshold,
        cv::Vec4d& params);
    
	void LetterBox(const cv::Mat& image, cv::Mat& outImage,
        cv::Vec4d& params, //[ratiox,ratioy,dw,dh]
        const cv::Size& newShape = cv::Size(640, 640),
        bool autoShape = false,
        bool scaleFill = false,
        bool scaleUp = true,
        int stride = 32,
        const cv::Scalar& color = cv::Scalar(114, 114, 114));
    
	std::wstring charToWstring(const char* str);
    
	static void getBestClassInfo(std::vector<float>::iterator it, const int& numClasses, float& bestConf, int& bestClassId);
    
	size_t vectorProduct(const std::vector<int64_t>& vector);


    std::vector<const char*> inputNames;
    std::vector<const char*> outputNames;
    bool isDynamicInputShape;
    cv::Size2f inputImageShape;
};

namespace utils
{
    std::vector<std::string> loadNames(const std::string& path);

}
