#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

cv::Mat createGammaLUT(double gamma){
    double inverse_gamma = 1.0 / gamma;

    cv::Mat lut_matrix(1, 256, CV_8UC1 );
    unsigned char* ptr = lut_matrix.ptr();
    for( int i = 0; i < 256; i++ )
        ptr[i] = (int)( pow( (double) i / 255.0, inverse_gamma ) * 255.0 );

    return lut_matrix;
}


void changeHSV(cv::Mat& image, double hFactor, double sFactor, double vFactor){
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, CV_BGR2HSV);
    std::vector<cv::Mat> planes;
    cv::split(hsvImage, planes);
    planes[0] *= hFactor;
    planes[1] *= sFactor;
    planes[2] *= vFactor;
    cv::merge(planes, hsvImage);
    cv::cvtColor(hsvImage, image, CV_HSV2BGR);
}

int main(int argc, char const* argv[])
{
    cv::VideoCapture cap(0);
    cv::Mat frame;
    cv::Mat frameRefined;
    int widthTile = 100;
    int heightTile = 60;
    int tileSize = 24;

    cv::Mat masterImage(cv::Size(widthTile * 3, heightTile * 3), CV_8UC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);
    ImageCollections imageCollections("image");

    cv::Mat gammaLUT = createGammaLUT(std::stof(std::string(argv[1])));
    double hf = std::stof(std::string(argv[2]));
    double sf = std::stof(std::string(argv[3]));
    double vf = std::stof(std::string(argv[4]));

    int tickCount = 0;
    auto startTime = std::chrono::system_clock::now();
    while (cv::waitKey(1) != 'q') {
        auto nowTime = std::chrono::system_clock::now(); 
        auto dur = nowTime - startTime;
        double fps = (double)tickCount++ / (std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()) * 1000;
        std::cout << std::fixed << "fps: " << fps << std::endl;

        cap >> frame;
        changeHSV(frame, hf, sf, vf);
        cv::LUT(frame, gammaLUT, frameRefined);
        //frame = cv::imread("/Users/kazuhiro/Pictures/screenshots/a.png");
        // frame = cv::imread("/Users/kazuhiro/sandbox/4646fa4bc5109.jpg");


        cv::resize(frameRefined, masterImage, masterImage.size());

        for(int i = 0; i < heightTile; ++i){
            for(int j = 0; j < widthTile; ++j){
                cv::Mat croppedTmp = masterImage(cv::Rect(j*3, i*3, 3, 3));
                cv::Mat cropped;
                croppedTmp.convertTo(cropped, CV_8UC3);
                cv::Mat& nearestChip = imageCollections.findNearest(cropped);
                // outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)) = nearestChip;
                nearestChip.copyTo(outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)));
            }
        }
        cv::imshow("sample", outputImage);
        cv::imshow("master", masterImage);
    }

    return 0;
}

