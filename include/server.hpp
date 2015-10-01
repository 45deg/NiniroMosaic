#include <string>
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

struct Server {
private:
    int port;
    int tileSize;
public:
    Server(int _port, int _tileSize);
    void operator ()(ImageCollections& imageCollections);
private:
    std::string processImage(std::string inFile, ImageCollections& imageCollections);
    std::string makeOutputFilePath(std::string& inFile);
};
