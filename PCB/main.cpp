#include <iostream>
#include <opencv2/opencv.hpp>
#include"yolov5.h"

int main(int argc, char* argv[])
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    const std::string classNamesPath = "C:/Users/admin/Desktop/Defect-Detection/PCB/data/class.names";
    const std::string imagePath = "C:/Users/admin/Desktop/Defect-Detection/yolov5/data/images/ug470_7Series_Config_129.png";
    const std::string modelPath = "C:/Users/admin/Desktop/Defect-Detection/yolov5/runs/train/exp2/weights/best.onnx";
    const float confThreshold = 0.25f;
    const float iouThreshold = 0.45f;
    bool isGPU = false;

    const std::vector<std::string> classNames = utils::loadNames(classNamesPath);
    if (classNames.empty())
    {
        std::cerr << "Error: Empty class names file." << std::endl;
        return -1;
    }

    YOLODetector detector{ nullptr };
    cv::Mat image;
    std::vector<Output> result;

    try
    {
        detector = YOLODetector(modelPath, isGPU, cv::Size(640, 640));
        std::cout << "Model was initialized." << std::endl;

        image = cv::imread(imagePath);
        auto start = std::chrono::high_resolution_clock::now();
        result = detector.detect(image, confThreshold, iouThreshold);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "推理时间：" << duration << "ms" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    detector.draw(image, result, classNames);
    cv::namedWindow("result", cv::WINDOW_NORMAL);
    cv::imshow("result", image);
    cv::waitKey(0);
    cv::destroyAllWindows();
    system("pause");
    return 0;
}
