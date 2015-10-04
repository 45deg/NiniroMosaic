#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include "server.hpp"
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"
#include "cmdline.h"

void initParser(cmdline::parser& parser){
    parser.add<int>("height", 'h', "Number of rows of tiles", 100);
    parser.add<int>("width", 'w', "Number of columns of tiles", 60);
    parser.add<int>("size", 's', "Image size of tile", 24);
    parser.add<int>("precision", 'p', "Precision of tile", 5);
    parser.add<std::string>("directory", 'd', "Directory path of imagesets (absolute path)", "image");
}

void initDaemon(){
    // detatch from terminal
    if(fork() != 0) exit(0);
    // Be session group leader
    setsid();

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDOUT_FILENO);
}

int main(int argc, char * argv[])
{
    initDaemon();

    cmdline::parser args;
    initParser(args);
    args.parse_check(argc, argv);

    int widthTile = args.get<int>("width");
    int heightTile = args.get<int>("height");
    int tileSize = args.get<int>("size");
    int precision = args.get<int>("precision");

    const cv::Size masterSize = cv::Size(widthTile * precision, heightTile * precision);

    cv::Mat resizedFrame(masterSize, CV_8UC3);
    cv::Mat masterImage(masterSize, CV_32FC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);
    ImageCollections imageCollections(args.get<std::string>("directory"), tileSize, precision); 

    // run image conversion server
    std::thread server(Server(8080, tileSize, precision), std::ref(imageCollections));
    server.join();

    return 0;
}

