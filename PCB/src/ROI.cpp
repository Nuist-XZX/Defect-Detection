#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>
#include<stdexcept>

using namespace std;
using namespace cv;

// 定义ROI返回结构体：存放裁剪图像 + 矩形坐标
struct PcbRoi
{
    Mat roiImage;    // 裁剪后的PCB图像
    Rect roiRect;    // ROI矩形区域
};

/**
 * @brief 动态提取PCB感兴趣区域
 * @param src 输入原始BGR图像
 * @param minAreaRatio 有效轮廓最小面积占原图比例，过滤杂质
 * @param expandPixel 边缘扩边像素，补偿板材偏移
 * @return PcbRoi ROI结果
 */
PcbRoi extractPcbDynamicRoi(const Mat& src, double minAreaRatio = 0.05, int expandPixel = 5)
{
    // 空图像校验
    if (src.empty())
    {
        throw runtime_error("Input image is empty!");
    }

    Mat grayImg, blurImg, binaryImg;

    // 1. ROI定位预处理（仅用于轮廓检测，不修改原图）
    cvtColor(src, grayImg, COLOR_BGR2GRAY);          // 转灰度图
    GaussianBlur(grayImg, blurImg, Size(5, 5), 0);    // 轻度高斯降噪

    // 自适应二值化，适配光照不均场景
    adaptiveThreshold(
        blurImg,
        binaryImg,
        255,
        ADAPTIVE_THRESH_GAUSSIAN_C,
        THRESH_BINARY_INV,
        15,
        3
    );

    // 形态学闭运算，修补轮廓间隙
    Mat closeKernel = getStructuringElement(MORPH_RECT, Size(5, 5)); // 闭运算的结构核大小
    morphologyEx(binaryImg, binaryImg, MORPH_CLOSE, closeKernel, Point(-1, -1), 1);

    // 2. 轮廓检测
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binaryImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty())
    {
        throw runtime_error("No contour detected!");
    }

    // 3. 轮廓筛选：过滤小面积杂质
    int totalPixel = src.cols * src.rows;
    vector<pair<double, vector<Point>>> validContours;
    for (const auto& cnt : contours)
    {
        double area = contourArea(cnt);
        if (area > totalPixel * minAreaRatio)
        {
            validContours.emplace_back(area, cnt);
        }
    }

    if (validContours.empty())
    {
        throw runtime_error("No valid PCB contour!");
    }

    // 按面积降序，取最大轮廓（PCB主体）
    sort(validContours.begin(), validContours.end(),
        [](const pair<double, vector<Point>>& a, const pair<double, vector<Point>>& b)
        {
            return a.first > b.first;
        });

    // 计算PCB外接矩形
    Rect pcbRect = boundingRect(validContours[0].second);

    // 4. 边缘扩边 + 边界防越界
    int x = max(0, pcbRect.x - expandPixel);
    int y = max(0, pcbRect.y - expandPixel);
    int w = min(src.cols, pcbRect.x + pcbRect.width + expandPixel) - x;
    int h = min(src.rows, pcbRect.y + pcbRect.height + expandPixel) - y;

    Rect finalRoi(x, y, w, h);

    // 5. 裁剪原图，得到最终ROI
    PcbRoi result;
    result.roiRect = finalRoi;
    result.roiImage = src(finalRoi).clone();

    return result;
}


int main()
{
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);

    // 读取图像
    Mat src = imread("C:/Users/admin/Desktop/PCB-31.jpg");
    if (src.empty())
    {
        printf("Read image failed!\n");
        return -1;
    }

    try
    {
        // 第一步：动态提取ROI
        PcbRoi roiData = extractPcbDynamicRoi(src, 0.05, 5);
        Mat roi = roiData.roiImage;
		
        // 可视化调试
        rectangle(src, roiData.roiRect, Scalar(0, 0, 255), 2);

        namedWindow("Original Image", WINDOW_FREERATIO);
        namedWindow("PCB ROI", WINDOW_FREERATIO);

        imshow("Original Image", src);
        imshow("PCB ROI", roi);

        waitKey(0);
        destroyAllWindows();
    }
    catch (const runtime_error& e)
    {
        printf("Error: %s\n", e.what());
    }

    return 0;
}