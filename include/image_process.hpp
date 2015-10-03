#pragma once

#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

void makeMosaicArt(cv::Mat& masterImage, cv::Mat& outputImage,
                  ImageCollections& imageCollections,
                  int widthTile, int heightTile, int tileSize);
