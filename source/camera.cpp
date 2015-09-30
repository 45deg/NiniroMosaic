#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

void equalizeHistgram(cv::Mat& image){
    std::vector<cv::Mat> planes;
    cv::Mat imgHistEqualized;
    cv::cvtColor(image, imgHistEqualized, CV_BGR2YCrCb);
    cv::split(imgHistEqualized, planes);

    cv::equalizeHist(planes[0], planes[0]);

    cv::merge(planes, imgHistEqualized);
    cv::cvtColor(imgHistEqualized, image, CV_YCrCb2BGR);
}

int main(int argc, char const* argv[])
{
    std::cout << "Set up camera ..." << std::endl;

    cv::VideoCapture cap(0);
    cv::Mat frame;
    cv::Mat frameRefined;
    int widthTile = 100;
    int heightTile = 60;
    int tileSize = 24;

    const cv::Size masterSize = cv::Size(widthTile * 3, heightTile * 3);
        
    cv::Mat resizedFrame(masterSize, CV_8UC3);
    cv::Mat masterImage(masterSize, CV_32FC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);
    ImageCollections imageCollections("image", tileSize);

    int tickCount = 0;
    auto startTime = std::chrono::system_clock::now();
    while (cv::waitKey(1) != 'q') {
        auto nowTime = std::chrono::system_clock::now(); 
        double fps = (double)tickCount++ /
                     (std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime).count()) * 1000;
        std::cout << std::fixed << "fps: " << fps;

        cap >> frame;
        cv::resize(frame, resizedFrame, masterSize);
        // equalizeHistgram(resizedFrame);
        resizedFrame.convertTo(masterImage, CV_32FC3, 1./256);
        cv::cvtColor(masterImage, masterImage, CV_BGR2Lab);

        nowTime = std::chrono::system_clock::now(); 
        for(int i = 0; i < heightTile; ++i){
            for(int j = 0; j < widthTile; ++j){
                cv::Mat cropped = masterImage(cv::Rect(j*3, i*3, 3, 3)).clone();

                cv::Mat nearestChip = imageCollections.findNearest(cropped);
                nearestChip.copyTo(outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)));
            }
        }
        std::cout << " ImageProcessing: " <<
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now() - nowTime
                     ).count()
                  << "msec"
                  << std::endl;

        cv::imshow("sample", outputImage);
        cv::imshow("master", resizedFrame);
    }

    return 0;
}

