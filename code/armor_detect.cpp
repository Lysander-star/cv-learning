#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "用法: ./armor_detect <图片路径>" << std::endl;
        return -1;
    }
    cv::Mat src = cv::imread(argv[1]);
    if (src.empty()) {
        std::cerr << "无法读取图片" << std::endl;
        return -1;
    }
    std::cout << "图像尺寸: " << src.cols << " x " << src.rows
              << " 通道: " << src.channels() << std::endl;
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask_red, mask_blue;
    cv::inRange(hsv, cv::Scalar(0,120,100), cv::Scalar(10,255,255), mask_red);
    cv::inRange(hsv, cv::Scalar(100,120,100), cv::Scalar(140,255,255), mask_blue);
    cv::Mat mask;
    cv::bitwise_or(mask_red, mask_blue, mask);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::Mat opened;
    cv::morphologyEx(mask, opened, cv::MORPH_OPEN, kernel);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(opened, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::cout << "候选轮廓: " << contours.size() << std::endl;
    cv::Mat result = src.clone();
    int count = 0;
    for (const auto& c : contours) {
        if (cv::contourArea(c) < 30) continue;
        cv::Rect r = cv::boundingRect(c);
        double ratio = (double)std::max(r.width,r.height)/std::min(r.width,r.height);
        if (ratio < 2.0) continue;
        cv::rectangle(result, r, cv::Scalar(0,255,0), 2);
        std::cout << "灯条 " << ++count << ": (" << r.x << "," << r.y
                  << ") " << r.width << "x" << r.height << std::endl;
    }
    std::cout << "筛出灯条: " << count << std::endl;
    cv::imwrite("result.jpg", result);
    cv::imwrite("mask.jpg", opened);
    return 0;
}
