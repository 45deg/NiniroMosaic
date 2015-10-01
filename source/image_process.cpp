#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

void makeMosaicArt(cv::Mat& masterImage, cv::Mat& outputImage,
                  ImageCollections& imageCollections,
                  int widthTile, int heightTile, int tileSize){
    for(int i = 0; i < heightTile; ++i){
        for(int j = 0; j < widthTile; ++j){
            cv::Mat cropped = masterImage(cv::Rect(j*3, i*3, 3, 3)).clone();

            cv::Mat nearestChip = imageCollections.findNearest(cropped);
            nearestChip.copyTo(outputImage(cv::Rect(j*tileSize, i*tileSize, tileSize, tileSize)));
        }
    }
}
