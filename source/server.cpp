#include "server.hpp"
#include <string>
#include <thread>
#include "zmq.hpp"
#include "image_manager.hpp"
#include "image_process.hpp"
#include "opencv2/opencv.hpp"

Server::Server(int _port, int _tileSize)
{
    port = _port;
    tileSize = _tileSize;
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
        // std::cout << "Received " << requestData << " " << data << std::endl;

        std::string responseData = processImage(requestData, imageCollections);

        // send response
        int messageSize = responseData.size();
        zmq::message_t reply (messageSize);
        memcpy ((void *)reply.data(), responseData.c_str(), messageSize);
        socket.send (reply);
    }
}

std::string Server::processImage(std::string inFile, ImageCollections& imageCollections) {
    cv::Mat image = cv::imread(inFile);

    int widthTile = image.size().width / tileSize;
    int heightTile = image.size().height / tileSize;

    const cv::Size masterSize = cv::Size(widthTile * 3, heightTile * 3);

    cv::Mat resizedFrame(masterSize, CV_8UC3);
    cv::Mat masterImage(masterSize, CV_32FC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);

    cv::resize(image, resizedFrame, masterSize);
    resizedFrame.convertTo(masterImage, CV_32FC3, 1./256);
    cv::cvtColor(masterImage, masterImage, CV_BGR2Lab);

    makeMosaicArt(masterImage, outputImage, imageCollections,
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
    return toExt + "_out" + ext;
}
