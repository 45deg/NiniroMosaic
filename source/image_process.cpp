#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

void makeMosaicArt(cv::Mat& masterImage, cv::Mat& outputImage,
                  ImageCollections& imageCollections,
                  int widthTile, int heightTile, int tileSize){
    const int dw = masterImage.size().width  / widthTile;
    const int dh = masterImage.size().height / heightTile;

    for(int i = 0; i < heightTile; ++i){
        for(int j = 0; j < widthTile; ++j){
            cv::Mat cropped = masterImage(cv::Rect(j*dw, i*dh, dw, dh)).clone();

            cv::Mat nearestChip = imageCollections.findNearest(cropped);
            nearestChip.copyTo(outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)));
        }
    }
}
