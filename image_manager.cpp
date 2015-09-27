#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include "opencv2/opencv.hpp"
#include <vector>
#include <climits>

struct Tile {
    cv::Mat image;
    cv::Mat colorInfo;

    Tile(std::string filename){
        image = cv::imread(filename);
        colorInfo = cv::Mat(3, 3, image.type());

        int cropWidth = image.cols / 3;
        int cropHeight = image.rows / 3;

        for(int i = 0; i < 3; ++i){
            for(int j = 0; j < 3; ++j){
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
        
        std::cout << cv::format(colorInfo, "python") << std::endl;
    }
};

class ImageCollections {
private:
    std::vector<std::shared_ptr<Tile> > images;

public:
    ImageCollections(std::string dirName){
        std::vector<std::string> files = getListOfFiles(dirName);

        for (auto&& file : files) {
            images.push_back(std::make_shared<Tile>(file));
        }
    }

    cv::Mat& findNearest(cv::Mat& color) {
        unsigned int distMin = UINT_MAX;
        int argMin = -1;
        for (int index = 0; index < images.size(); index++) {
            unsigned int dist = 0;
            cv::Mat& targetColor = images[index]->colorInfo;

            int length = color.rows * color.step;
            for(int i = 0; i < length; ++i) {
                int diff = color.data[i] - targetColor.data[i];
                dist += diff * diff;
            }

            if (dist < distMin){
                distMin = dist;
                argMin = index;
            }
        }

        return images[argMin]->image;
    }

private:
    std::vector<std::string> getListOfFiles(std::string dirName){
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
};
