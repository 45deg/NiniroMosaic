#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include "image_manager.hpp"
#include "image_process.hpp"
#include "opencv2/opencv.hpp"
#include "cmdline.h"

void initParser(cmdline::parser& parser){
    parser.add<int>("height", 'h', "Number of rows of tiles", 100);
    parser.add<int>("width", 'w', "Number of columns of tiles", 60);
    parser.add<int>("size", 's', "Image size of tile", 24);
    parser.add<std::string>("directory", 'd', "Directory path of imagesets", "image");
}

int main(int argc, char * argv[])
{
    cmdline::parser args;
    initParser(args);
    args.parse_check(argc, argv);

    std::cout << "Set up camera ..." << std::endl;

    cv::VideoCapture cap(0);
    cv::Mat frame;
    cv::Mat frameRefined;
    int widthTile = args.get<int>("width");
    int heightTile = args.get<int>("height");
    int tileSize = args.get<int>("size");

    const cv::Size masterSize = cv::Size(widthTile * 3, heightTile * 3);
        
    cv::Mat resizedFrame(masterSize, CV_8UC3);
    cv::Mat masterImage(masterSize, CV_32FC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);
    ImageCollections imageCollections(args.get<std::string>("directory"), tileSize);

    int tickCount = 0;
    auto startTime = std::chrono::system_clock::now();
    while (cv::waitKey(1) != 'q') {
        auto nowTime = std::chrono::system_clock::now(); 
        double fps = (double)tickCount++ /
                     (std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime).count()) * 1000;
        std::cout << std::fixed << "fps: " << fps;

        cap >> frame;
        cv::resize(frame, resizedFrame, masterSize);
        resizedFrame.convertTo(masterImage, CV_32FC3, 1./256);
        cv::cvtColor(masterImage, masterImage, CV_BGR2Lab);

        makeMosaicArt(masterImage, outputImage, imageCollections,
                      widthTile, heightTile, tileSize);

        nowTime = std::chrono::system_clock::now(); 
        std::cout << " ImageProcessing: " <<
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now() - nowTime
                     ).count()
                  << "msec"
                  << std::endl;

        cv::imshow("sample", outputImage);
    }

    return 0;
}

