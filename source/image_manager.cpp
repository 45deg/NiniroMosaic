#include "image_manager.hpp"
#include <iostream>
#include <memory>
#include <dirent.h>
#include <sys/types.h>
#include "opencv2/opencv.hpp"
#include <vector>
#include <climits>

Tile::Tile(std::string filename, int tileSize, int presicion){

    image = cv::imread(filename, 1);
    cv::resize(image, image, cv::Size(tileSize, tileSize));

    colorInfo = cv::Mat(presicion, presicion, CV_8UC3);

    int cropWidth = image.cols / presicion;
    int cropHeight = image.rows / presicion;

    for(int i = 0; i < presicion; ++i){
        for(int j = 0; j < presicion; ++j){
            double r = 0, g = 0, b = 0;

            for(int y = cropHeight * i; y < cropHeight * (i+1); ++y){
                for(int x = cropWidth * j; x < cropWidth * (j+1); ++x){
                    int offset = y * image.step + x * image.elemSize();

                    b += image.data[offset+0];
                    g += image.data[offset+1];
                    r += image.data[offset+2];
                }
            }

            b /= cropHeight * cropWidth;
            g /= cropHeight * cropWidth;
            r /= cropHeight * cropWidth;

            colorInfo.at<cv::Vec3b>(i,j) = cv::Vec3b(b, g, r);
        }
    }

    // std::cout << "Type:" << image.type() << ' ' << cv::format(colorInfo, "python") << std::endl;
}

ImageCollections::ImageCollections(std::string dirName, int tileSize, int presicion){
    std::vector<std::string> files = getListOfFiles(dirName);

    for (auto&& file : files) {
        auto tile = std::make_shared<Tile>(file, tileSize, presicion);
        if(tile->image.data != NULL) {
            images.push_back(tile);
        }
        std::cout << "\033[0G" << images.size() << " images loaded" << std::flush;
    }

    std::cout << std::endl;
    std::cout << "Making feature..." << std::endl;

    cv::Mat featureMat(images.size(), presicion*presicion*3, CV_32FC1);
    for(int i = 0; i < images.size(); ++i){
        cv::Mat lab;
        images[i]->colorInfo.convertTo(lab, CV_32FC3, 1./256);

        cv::cvtColor(lab, lab, CV_BGR2Lab);

        lab.reshape(1,1).copyTo(featureMat.row(i));
    }

    std::cout << "Making index..." << std::endl;

    kdtree = std::make_shared<cv::flann::Index>(featureMat, cv::flann::KDTreeIndexParams(4));
}

cv::Mat ImageCollections::findNearest(cv::Mat& color) {
    cv::Mat query = color.reshape(1, 1);
    cv::Mat index;
    cv::Mat dist;
    static auto searchParams = cv::flann::SearchParams(32);
    
    kdtree->knnSearch(query, index, dist, 1, searchParams);

    return images[index.at<int>(0,0)]->image;
}

std::vector<std::string> ImageCollections::getListOfFiles(std::string dirName){
    std::vector<std::string> files;

    DIR* dp = opendir(dirName.c_str());
    std::string prefix = dirName + "/";
    if (dp != NULL)
    {
        struct dirent* dent;
        while((dent = readdir(dp)) != NULL){
            // skip hidden file
            if(dent->d_name[0] == '.')
                continue;
            files.push_back(prefix + std::string(dent->d_name));
        }
        closedir(dp);
    }

    return files;
}
