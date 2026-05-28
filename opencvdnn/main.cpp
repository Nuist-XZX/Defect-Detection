#include <iostream>
#include <opencv2/opencv.hpp>
#include "ImageReader.h"
#include "PreProcessor.h"
#include "YoloV5Detector.h"

using namespace std;

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    // 1. 读取图片
    ImageReader reader;
    reader.open("C:/Users/admin/Desktop/Defect-Detection/yolov5/data/images/ug470_7Series_Config_129.png");

    // 2. 加载 YOLOv5 6.0 onnx
    YoloV5Detector detector;
    detector.loadModel("C:/Users/admin/Desktop/Defect-Detection/yolov5/runs/train/exp2/weights/best.onnx");

    PreProcessor pre;

    cv::Mat frame = reader.getFrame();

    // 3. 推理
    auto results = detector.detect(frame, pre);
    cout << "检测到目标数量：" << results.size() << endl;

    // 4. 画框
    for (auto& res : results) {
        cv::rectangle(frame, res.box, cv::Scalar(0, 0, 255), 2);

        string text = "id:" + to_string(res.class_id) + " " + to_string((int)(res.confidence * 100)) + "%";

        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 2;   // 字体大小
        int thickness = 2;      // 字体粗细
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);

        cv::Point textOrg(res.box.x, res.box.y - 5);
        if (textOrg.y < textSize.height) {
            textOrg.y = res.box.y + textSize.height + 5;
        }
        if (textOrg.x + textSize.width > frame.cols) {
            textOrg.x = frame.cols - textSize.width - 5;
        }
        if (textOrg.x < 0) textOrg.x = 5;

        // 红色背景
        cv::rectangle(frame,
            cv::Rect(textOrg.x, textOrg.y - textSize.height,
                textSize.width, textSize.height + baseline),
            cv::Scalar(0, 0, 255), cv::FILLED);
        // 白色文字
        cv::putText(frame, text, textOrg, fontFace, fontScale, cv::Scalar(255, 255, 255), thickness);
    }

    cv::namedWindow("result", cv::WINDOW_FREERATIO);

    cv::imshow("result", frame);
    //cv::imwrite("C:/Users/admin/Desktop/12.png", frame);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}