#include"yolov5.h"

YOLODetector::YOLODetector(const std::string& modelPath,
    const bool& isGPU = true,
    const cv::Size& inputSize = cv::Size(640, 640))
{   //环境初始化
    env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "ONNX_DETECTION");
    sessionOptions = Ort::SessionOptions();
    sessionOptions.SetIntraOpNumThreads(4);//线程数
    //检测GPU
    std::vector<std::string> availableProviders = Ort::GetAvailableProviders();
    auto cudaAvailable = std::find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");
    OrtCUDAProviderOptions cudaOption;

    if (isGPU && (cudaAvailable == availableProviders.end()))
    {
        std::cout << "Inference device: CPU" << std::endl;
    }
    else if (isGPU && (cudaAvailable != availableProviders.end()))
    {
        std::cout << "Inference device: GPU" << std::endl;
        sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
    }
    else
    {
        std::cout << "Inference device: CPU" << std::endl;
    }

#ifdef _WIN32
    //
    std::wstring w_modelPath = this->charToWstring(modelPath.c_str());
    session = Ort::Session(env, w_modelPath.c_str(), sessionOptions);
#else
    session = Ort::Session(env, modelPath.c_str(), sessionOptions);
#endif

    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
    std::vector<int64_t> inputTensorShape = inputTypeInfo.GetTensorTypeAndShapeInfo().GetShape();
    this->isDynamicInputShape = false;
    // 检查是否是动态宽高
    if (inputTensorShape[2] == -1 && inputTensorShape[3] == -1)
    {
        std::cout << "Dynamic input shape" << std::endl;
        this->isDynamicInputShape = true;
    }

    for (auto shape : inputTensorShape)
        std::cout << "Input shape: " << shape << std::endl;

    this->inputImageShape = cv::Size2f(inputSize);
}
//前处理
void YOLODetector::preprocessing(cv::Mat& image, float*& blob, cv::Vec4d& params, std::vector<int64_t>& inputTensorShape)
{
    cv::Mat resizedImage, floatImage;
    cv::cvtColor(image, resizedImage, cv::COLOR_BGR2RGB);

    YOLODetector::LetterBox(resizedImage, resizedImage,
        params, this->inputImageShape,
        this->isDynamicInputShape, false, true,
        32, cv::Scalar(114, 114, 114));

    inputTensorShape[2] = resizedImage.rows;
    inputTensorShape[3] = resizedImage.cols;

    resizedImage.convertTo(floatImage, CV_32FC3, 1 / 255.0);
    blob = new float[floatImage.cols * floatImage.rows * floatImage.channels()];
    cv::Size floatImageSize{ floatImage.cols, floatImage.rows };

    // hwc to chw
    std::vector<cv::Mat> chw(floatImage.channels());
    for (int i = 0; i < floatImage.channels(); ++i)
    {
        chw[i] = cv::Mat(floatImageSize, CV_32FC1, blob + i * floatImageSize.width * floatImageSize.height);
    }
    cv::split(floatImage, chw);
}
//后处理
std::vector<Output> YOLODetector::postprocessing(const cv::Size& resizedImageShape,
    const cv::Size& originalImageShape,
    std::vector<Ort::Value>& outputTensors,
    const float& confThreshold, const float& iouThreshold, cv::Vec4d& params)
{
    std::vector<cv::Rect> boxes;
    std::vector<float> confs;
    std::vector<int> classIds;

    auto* rawOutput = outputTensors[0].GetTensorData<float>();
    std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
    size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
    std::vector<float> output(rawOutput, rawOutput + count);

    int numClasses = (int)outputShape[2] - 5;
    int elementsInBatch = (int)(outputShape[1] * outputShape[2]);

    for (auto it = output.begin(); it != output.begin() + elementsInBatch; it += outputShape[2])
    {
        float clsConf = it[4];

        if (clsConf > confThreshold)
        {
            int centerX = (int)(it[0] - params[2]) / params[0];
            int centerY = (int)(it[1] - params[3]) / params[1];
            int width = (int)(it[2]) / params[0];
            int height = (int)(it[3] / params[1]);
            int left = centerX - width / 2;
            int top = centerY - height / 2;

            float objConf;
            int classId;
            this->getBestClassInfo(it, numClasses, objConf, classId);

            float confidence = clsConf * objConf;

            boxes.emplace_back(left, top, width, height);
            confs.emplace_back(confidence);
            classIds.emplace_back(classId);
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confs, confThreshold, iouThreshold, indices);
    std::vector<Output> detections;
    for (int idx : indices)
    {
        Output det;
        det.box = cv::Rect(boxes[idx]);
        det.conf = confs[idx];
        det.class_index = classIds[idx];
        detections.emplace_back(det);
    }

    return detections;
}

std::vector<Output> YOLODetector::detect(cv::Mat& image, const float& confThreshold = 0.4,
    const float& iouThreshold = 0.45)
{
    float* blob = nullptr;
    cv::Vec4d params;
    std::vector<int64_t> inputTensorShape{ 1, 3, -1, -1 };
    this->preprocessing(image, blob, params, inputTensorShape);
    //计算输出尺寸1x3x640x640
    size_t inputTensorSize = this->vectorProduct(inputTensorShape);

    std::vector<float> inputTensorValues(blob, blob + inputTensorSize);

    std::vector<Ort::Value> inputTensors;

    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    inputTensors.push_back(Ort::Value::CreateTensor<float>(
        memoryInfo, inputTensorValues.data(), inputTensorSize,
        inputTensorShape.data(), inputTensorShape.size()
        ));
    //获取输入阶段和输出节点名称
    Ort::AllocatorWithDefaultOptions allocator;
    Ort::AllocatedStringPtr input_name_Ptr = session.GetInputNameAllocated(0, allocator);
    //std::cout << input_name_Ptr << std::endl;
    inputNames.push_back(input_name_Ptr.get());

    Ort::AllocatedStringPtr output_name_Ptr = session.GetOutputNameAllocated(0, allocator);
    outputNames.push_back(output_name_Ptr.get());

    const char* input_names[] = { inputNames[0] };
    const char* output_names[] = { outputNames[0] };
    std::cout << "Input name: " << inputNames[0] << std::endl;
    std::cout << "Output name: " << outputNames[0] << std::endl;

    std::vector<Ort::Value> outputTensors = this->session.Run(Ort::RunOptions{ nullptr },
        input_names,
        inputTensors.data(),
        1,
        output_names,
        1);

    cv::Size resizedShape = cv::Size((int)inputTensorShape[3], (int)inputTensorShape[2]);
    std::vector<Output> result = this->postprocessing(resizedShape,
        image.size(),
        outputTensors,
        confThreshold, iouThreshold, params);

    delete[] blob;

    return result;
}//并计算该向量（数组）中所有元素的乘积
size_t YOLODetector::vectorProduct(const std::vector<int64_t>& vector)
{
    if (vector.empty())
        return 0;

    size_t product = 1;
    for (const auto& element : vector)
        product *= element;

    return product;
}
//将一个以 char* 表示的 UTF-8 编码的字符串转换为 std::wstring宽字符
std::wstring YOLODetector::charToWstring(const char* str)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.from_bytes(str);
}

void YOLODetector::draw(cv::Mat& image, std::vector<Output>& detections,
    const std::vector<std::string>& classNames)
{
    for (const Output& detection : detections)
    {
        cv::rectangle(image, detection.box, cv::Scalar(229, 160, 21), 2);

        int x = detection.box.x;
        int y = detection.box.y;

        int conf = (int)std::round(detection.conf * 100);
        int classId = detection.class_index;
        std::string label = classNames[classId] + " 0." + std::to_string(conf);

        int baseline = 0;
        cv::Size size = cv::getTextSize(label, cv::FONT_ITALIC, 0.8, 2, &baseline);
        cv::rectangle(image,
            cv::Point(x, y - 25), cv::Point(x + size.width, y),
            cv::Scalar(229, 160, 21), -1);

        cv::putText(image, label,
            cv::Point(x, y - 3), cv::FONT_ITALIC,
            0.8, cv::Scalar(255, 255, 255), 2);
    }
}

void YOLODetector::LetterBox(const cv::Mat& image, cv::Mat& outImage,
    cv::Vec4d& params, const cv::Size& newShape,
    bool autoShape, bool scaleFill, bool scaleUp,
    int stride, const cv::Scalar& color)
{
    cv::Size shape = image.size();
    float r = std::min((float)newShape.height / (float)shape.height,
        (float)newShape.width / (float)shape.width);
    if (!scaleUp)
        r = std::min(r, 1.0f);

    float ratio[2]{ r, r };
    int new_un_pad[2] = { (int)std::round((float)shape.width * r),(int)std::round((float)shape.height * r) };

    auto dw = (float)(newShape.width - new_un_pad[0]);
    auto dh = (float)(newShape.height - new_un_pad[1]);

    if (autoShape)
    {
        dw = (float)((int)dw % stride);
        dh = (float)((int)dh % stride);
    }
    else if (scaleFill)
    {
        dw = 0.0f;
        dh = 0.0f;
        new_un_pad[0] = newShape.width;
        new_un_pad[1] = newShape.height;
        ratio[0] = (float)newShape.width / (float)shape.width;
        ratio[1] = (float)newShape.height / (float)shape.height;
    }

    dw /= 2.0f;
    dh /= 2.0f;

    if (shape.width != new_un_pad[0] && shape.height != new_un_pad[1])
    {
        cv::resize(image, outImage, cv::Size(new_un_pad[0], new_un_pad[1]));
    }
    else {
        outImage = image.clone();
    }

    int top = int(std::round(dh - 0.1f));
    int bottom = int(std::round(dh + 0.1f));
    int left = int(std::round(dw - 0.1f));
    int right = int(std::round(dw + 0.1f));
    params[0] = ratio[0];
    params[1] = ratio[1];
    params[2] = left;
    params[3] = top;
    cv::copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color);
}

void YOLODetector::getBestClassInfo(std::vector<float>::iterator it, const int& numClasses,
    float& bestConf, int& bestClassId)
{
    bestClassId = 5;
    bestConf = 0;

    for (int i = 5; i < numClasses + 5; i++)
    {
        if (it[i] > bestConf)
        {
            bestConf = it[i];
            bestClassId = i - 5;
        }
    }

}
//读取classname文件
std::vector<std::string> utils::loadNames(const std::string& path)
{
    // load class names
    std::vector<std::string> classNames;
    std::ifstream infile(path);
    if (infile.good())
    {
        std::string line;
        while (getline(infile, line))
        {
            if (line.back() == '\r')
                line.pop_back();
            classNames.emplace_back(line);
        }
        infile.close();
    }
    else
    {
        std::cerr << "ERROR: Failed to access class name path: " << path << std::endl;
    }

    return classNames;
}
