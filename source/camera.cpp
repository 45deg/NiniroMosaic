#include <iostream>
#include <iomanip>
#include <chrono>
#include "opencv2/opencv.hpp"

int main()
{
    cv::VideoCapture cap(0);
    cv::Mat frame;

    int widthTile = 16*3;
    int heightTile = 9*3;

    cv::Mat masterImage(cv::Size(widthTile, heightTile), CV_8UC1);

    int tickCount = 0;
    auto startTime = std::chrono::system_clock::now();
    while (cv::waitKey(1) != 'q') {
        auto nowTime = std::chrono::system_clock::now(); 
        auto dur = nowTime - startTime;
        double fps = (double)tickCount++ / (std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()) * 1000;
        std::cout << std::fixed << "fps: " << fps << std::endl;
        std::cout << "mapsize: " << frame.size() << std::endl;

        cap >> frame;
        cv::resize(frame, masterImage, masterImage.size());
        cv::imshow("sample", masterImage);
    }

    return 0;
}

