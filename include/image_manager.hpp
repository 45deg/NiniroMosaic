#pragma once

#include <memory>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include "opencv2/opencv.hpp"
#include <vector>
#include <climits>
#include <mutex>

struct Tile {
    cv::Mat image;
    cv::Mat colorInfo;

    Tile(std::string, int, int);
};

class ImageCollections {
private:
    std::vector<std::shared_ptr<Tile> > images;
    std::shared_ptr<cv::flann::Index> kdtree;
    cv::Mat featureMat;

public:
    std::mutex mtx;
    ImageCollections(std::string, int, int);
    cv::Mat findNearest(cv::Mat& color);
    void makeMosaicArt(cv::Mat& masterImage, cv::Mat& outputImage,
                      int widthTile, int heightTile, int tileSize);

private:
    std::vector<std::string> getListOfFiles(std::string dirName);
};
