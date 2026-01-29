#pragma once

// 1. Extension Loaders FIRST
#include <GL/glew.h>

// 2. Windowing/System headers SECOND
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <array>

// 3. Complex libraries (OpenCV, etc.) LAST
#include <opencv2/opencv.hpp>

#include "logger.hpp"
#include "camera.hpp"

class GlRender
{
public:
    GlRender();
   ~GlRender();

    void setScale(float s);
    void setSize(int width, int height);
    void setFullscreen(bool fullscreen);
    void setVsync(bool vsync);
    void setMaximization(bool maximized);
    void setAntialiasing(bool antialiased);

    void getScreenshot() const;
    int getWidth() const;
    int getHeight() const;

    std::string version;
    std::string profile;
    std::string renderer;
    std::string vendor;
    std::string shadingLanguage;

    bool isVSynced() const;
    bool isFullscreen() const;
    bool isMaximized() const;
    bool isAntialiased() const;

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void init();

    // Variables for camera movement
    static Camera *cam; 
    static bool isMouseMoved;
    static float lastX;
    static float lastY;
    
    GLuint getTextureID(const cv::Mat &mat);
    GLFWwindow *window = nullptr;
private:
    std::string winname = "ICP";
    std::array<int,2>  winPos = {0,0};
    int  fps       = 60;
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
    bool maximized    = false;
    bool antialiased  = false;
    
    float winScale     = 0.5;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void handle_key_press(int key, int action);
    void onKeyEvent(int key, int action);

    void setImguiParameters();
    void setWindowHints();
    void setGlfwFeatures();
    void setGlfwWindowInstance();
    void setGlfwCallbacks();
};