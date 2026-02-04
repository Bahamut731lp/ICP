#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <opencv2/opencv.hpp>

class Video {
public:
    static void init();    
    static void shutdown(); 
    static bool getFrame(cv::Mat& frame);
    static int getTextureId();

private:
    static void loop(); // The actual thread function

    static std::thread worker;
    static std::mutex mutex;
    static std::atomic<bool> is_running;

    static cv::Mat frame;
    static bool is_frame_ready;
    static cv::VideoCapture capture; 
    static GLuint textureId;
};