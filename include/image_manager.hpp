#ifndef __IMAGE_MANAGER_HPP
#define __IMAGE_MANAGER_HPP

#include <memory>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include "opencv2/opencv.hpp"
#include <vector>
#include <climits>

struct Tile {
    cv::Mat image;
    cv::Mat colorInfo;

    Tile(std::string, int);
};

class ImageCollections {
private:
    std::vector<std::shared_ptr<Tile> > images;
    std::shared_ptr<cv::flann::Index> kdtree;

public:
    ImageCollections(std::string, int);
    cv::Mat findNearest(cv::Mat& color);

private:
    std::vector<std::string> getListOfFiles(std::string dirName);
};

#endif
