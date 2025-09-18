#pragma once

#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "../lib/logger.hpp"
#include <iostream>

class Window {
public:
    Window(int width, int height, const char* title, bool fullscreen = false, bool vsync = false);
    ~Window();
    
    GLFWwindow* getWindow() const;

    // Current Window Values
    int width;
    int height;

    // Window position & size restoration
    int lastWindowX;
    int lastWindowY;
    int lastWindowWidth;
    int lastWindowHeight;


    static bool isMouseMoved;
    static float lastX;
    static float lastY;

private:
    GLFWwindow* window;

    bool fullscreen;
    bool vsync;
};

#endif
