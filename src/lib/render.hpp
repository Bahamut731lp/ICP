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

enum CursorMode {
    LOCKED,
    FREE
};

class Renderer
{
public:
    Renderer();
   ~Renderer();

    static bool isVSynced();
    static bool isFullscreen();
    static bool isMaximized();
    static bool isAntialiased();

    static void getScreenshot();
    static int getWidth();
    static int getHeight();

    static void setScale(float s);
    static void setSize(int width, int height);
    static void setFullscreen(bool fullscreen);
    static void setVsync(bool vsync);
    static void setMaximization(bool maximized);
    static void setAntialiasing(bool antialiased);
    static void setCursor(CursorMode cursor);

    static std::string version;
    static std::string profile;
    static std::string renderer;
    static std::string vendor;
    static std::string shadingLanguage;

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    static void init();

    // Variables for camera movement
    static Camera *camera; 
    static bool isMouseMoved;
    static float lastX;
    static float lastY;
    
    GLuint getTextureID(const cv::Mat &mat);
    
    static GLFWwindow *window;
private:
    static std::string name;
    static std::array<int,2> position;
    static int winWidth;
    static int winHeight;
    static float winScale;

    // Window position & size restoration
    static int lastWindowX;
    static int lastWindowY;
    static int lastWindowWidth;
    static int lastWindowHeight;

    static bool fullscreen; 
    static bool vsync;
    static bool maximized;
    static bool antialiased;
    
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static void setImguiParameters();
    static void setWindowHints();
    static void setGlfwFeatures();
    static void setGlfwWindowInstance();
    static void setGlfwCallbacks();
};