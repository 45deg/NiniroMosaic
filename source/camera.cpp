#include <iostream>
#include <iomanip>
#include <chrono>
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

int main()
{
    cv::VideoCapture cap(0);
    cv::Mat frame;

    int widthTile = 16*2;
    int heightTile = 9*2;
    int tileSize = 24;

    cv::Mat masterImage(cv::Size(widthTile * 3, heightTile * 3), CV_8UC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);
    ImageCollections imageCollections("image");

    int tickCount = 0;
    auto startTime = std::chrono::system_clock::now();
    while (cv::waitKey(1) != 'q') {
        auto nowTime = std::chrono::system_clock::now(); 
        auto dur = nowTime - startTime;
        double fps = (double)tickCount++ / (std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()) * 1000;
        std::cout << std::fixed << "fps: " << fps << std::endl;

        cap >> frame;
        cv::resize(frame, masterImage, masterImage.size());
        for(int i = 0; i < heightTile; ++i){
            for(int j = 0; j < widthTile; ++j){
                cv::Mat cropped = masterImage(cv::Rect(j*3, i*3, 3, 3));
                cv::Mat& nearestChip = imageCollections.findNearest(cropped);
                // outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)) = nearestChip;
                nearestChip.copyTo(outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)));
            }
        }
        cv::imshow("sample", outputImage);
    }

    return 0;
}

