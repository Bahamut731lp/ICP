#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../lib/logger.hpp"

class GlRender
{
public:
    GlRender();
   ~GlRender();

    void setScale(float s);
    void setSize(int width, int height);
    void setFullscreen(bool fullscreen);
    void setVsync(bool vsync);

    bool isVSynced() const;
    bool isFullscreen() const;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  
    GLuint getTextureID(const cv::Mat &mat);
    void init();
    void draw(const cv::Mat& frame);

    GLFWwindow *window     = nullptr;
private:
    std::string winname = "ICP";
    std::array<int,2>  winPos = {0,0};
    int  fps       = 60;
    int  frameWidth   = 1280;
    int  frameHeight  = 720;
    int  winWidth     = 1280;
    int  winHeight    = 720;

    // Window position & size restoration
    int lastWindowX;
    int lastWindowY;
    int lastWindowWidth;
    int lastWindowHeight;

    bool initialized  = false;
    bool running      = false;
    bool fullscreen   = false;
    bool vsync        = false;
    
    float winScale     = 0.5;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void handle_key_press(int key, int action);
    void onKeyEvent(int key, int action);
};