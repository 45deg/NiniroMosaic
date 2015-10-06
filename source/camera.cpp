#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include "server.hpp"
#include "image_manager.hpp"
#include "opencv2/opencv.hpp"
#include "cmdline.h"

#include <GLFW/glfw3.h>
#include "glext.h"

void initParser(cmdline::parser& parser){
    parser.add<int>("height", 'h', "Number of rows of tiles", 100);
    parser.add<int>("width", 'w', "Number of columns of tiles", 60);
    parser.add<int>("size", 's', "Image size of tile", 24);
    parser.add<int>("precision", 'p', "Precision of tile", 5);
    parser.add<std::string>("directory", 'd', "Directory path of imagesets", "image");
    parser.add<std::string>("movie", 'm', "path of movie", false);
}

GLFWwindow* initOpenGLWindow(const cv::Size size, const std::string title){
    // Initializes the GLFW library.
    glfwInit();
    // Create Window
    GLFWwindow* window = glfwCreateWindow( size.width, size.height, title.c_str(), nullptr, nullptr );
    glfwMakeContextCurrent( window );

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    return window;
}

int main(int argc, char * argv[])
{
    cmdline::parser args;
    initParser(args);
    args.parse_check(argc, argv);


    int widthTile = args.get<int>("width");
    int heightTile = args.get<int>("height");
    int tileSize = args.get<int>("size");
    int precision = args.get<int>("precision");
    bool isMovie = args.exist("movie");

    const cv::Size masterSize = cv::Size(widthTile * precision, heightTile * precision);

    // Set up camera
    std::cout << "Set up camera ..." << std::endl;
    cv::VideoCapture cap;
    cv::Mat frame;

    if(isMovie) {
        cap.open(args.get<std::string>("movie"));
    } else {
        cap.open(0);
    }

    cv::Mat resizedFrame(masterSize, CV_8UC3);
    cv::Mat masterImage(masterSize, CV_32FC3);
    cv::Mat outputImage(cv::Size(widthTile * tileSize, heightTile * tileSize), CV_8UC3);
    ImageCollections imageCollections(args.get<std::string>("directory"), tileSize, precision); 

    // run image conversion server
    std::thread server(Server(8080, tileSize, precision), std::ref(imageCollections));
    server.detach();

    GLFWwindow* window = initOpenGLWindow(outputImage.size(), "Mosaic");

    auto prevTime = std::chrono::high_resolution_clock::now();

    // do until window close
    while(! glfwWindowShouldClose( window ) && cap.isOpened() )
    {
        // calc FPS
        auto curTime = std::chrono::high_resolution_clock::now(); 
        double fps = 1000./std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count();
        prevTime = curTime;
        std::cout << std::fixed << "fps: " << fps;

        // Prosessing image From camera
        cap >> frame;

        cv::resize(frame, resizedFrame, masterSize);

        if(!isMovie)
            cv::flip(resizedFrame, resizedFrame, 1);

        resizedFrame.convertTo(masterImage, CV_32FC3, 1./256);
        cv::cvtColor(masterImage, masterImage, CV_BGR2Lab);

        curTime = std::chrono::high_resolution_clock::now(); 

        imageCollections.makeMosaicArt(masterImage, outputImage,
                                       widthTile, heightTile, tileSize);

        std::cout << " ImageProcessing: " <<
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - curTime
                    ).count()
            << "msec"
            << std::endl;

        // Draw Image

        glClear(GL_COLOR_BUFFER_BIT);

        cv::flip(outputImage, outputImage, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, outputImage.size().width, outputImage.size().height,
                0, GL_BGR_EXT, GL_UNSIGNED_BYTE, outputImage.data);

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 1.0);
        glVertex2d(-1,1);
        glTexCoord2d(0.0, 0.0);
        glVertex2d(-1,-1);
        glTexCoord2d(1.0, 0.0);
        glVertex2d(1,-1);
        glTexCoord2d(1.0, 1.0);
        glVertex2d(1,1);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

