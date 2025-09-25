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

    int  start(cv::Mat img);

    void setScale(float s) {
        winScale = s;
    }

    void setFPS(float f) {
        fps = f;
    }

    void setFullscreen(bool fullscreen);
    void setVsync(bool vsync);

    bool isVSynced() const;
    bool isFullscreen() const;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    std::string winname = "ICP";
    std::array<int,2>  winPos = {0,0};
    int  fps       = 30;
    int  frameWidth   = 1920;
    int  frameHeight  = 1080;
    int  winWidth     = 1920;
    int  winHeight    = 1080;

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
    GLenum minFilter  = GL_LINEAR_MIPMAP_LINEAR;
    GLenum magFilter  = GL_LINEAR;
    GLenum wrapFilter = GL_CLAMP;

    GLFWwindow *window     = nullptr;

    GLuint getTextureID(const cv::Mat &mat);
    void init();
    void draw(const cv::Mat& frame);
  
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void handle_key_press(int key, int action);
    void onKeyEvent(int key, int action);
};