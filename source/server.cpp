#include "server.hpp"
#include <string>
#include <thread>
#include <iostream>
#include <chrono>
#include "zmq.hpp"
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"

Server::Server(int _port, int _tileSize, int _precision)
{
    port = _port;
    tileSize = _tileSize;
    precision = _precision;
}

// thread main
void Server::operator()(ImageCollections& imageCollections) {
    // prepare socket
    zmq::context_t context (1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:" + std::to_string(port));

    while (true) {
        zmq::message_t request;

        // wait for request
        socket.recv (&request);
        std::string requestData = std::string(static_cast<char*>(request.data())
                                             ,request.size());

        std::cout << "Received " << requestData;

        auto nowTime = std::chrono::system_clock::now();

        std::string responseData = processImage(requestData, imageCollections);

        std::cout << " ImageProcessing: " <<
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now() - nowTime
                     ).count()
                  << "msec"
                  << std::endl;

        // send response
        int messageSize = responseData.size();
        zmq::message_t reply (messageSize);
        memcpy ((void *)reply.data(), responseData.c_str(), messageSize);
        socket.send (reply);
    }
}

std::string Server::processImage(std::string inFile, ImageCollections& imageCollections) {
    cv::Mat image = cv::imread(inFile);

    int widthTile = image.size().width / tileSize * 2;
    int heightTile = image.size().height / tileSize * 2;

    const cv::Size masterSize = cv::Size(widthTile * precision, heightTile * precision);

    cv::Mat resizedFrame(masterSize, CV_8UC3);
    cv::Mat masterImage(masterSize, CV_32FC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);

    cv::resize(image, resizedFrame, masterSize);
    resizedFrame.convertTo(masterImage, CV_32FC3, 1./256);
    cv::cvtColor(masterImage, masterImage, CV_BGR2Lab);

    imageCollections.makeMosaicArt(masterImage, outputImage,
                                   widthTile, heightTile, tileSize);

    // output filename
    std::string outFile = makeOutputFilePath(inFile);

    cv::imwrite(outFile, outputImage);

    return outFile;
}

// hogehoge.ext -> hogehoge_out.ext
std::string Server::makeOutputFilePath(std::string& inFile){
    int dotPos = inFile.find_last_of('.');
    std::string toExt = inFile.substr(0, dotPos);
    std::string ext = inFile.substr(dotPos+1);
    return toExt + "_out." + ext;
}
