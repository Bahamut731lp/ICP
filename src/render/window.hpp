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
    
    void setFullscreen(bool fullscreen);
    void setVsync(bool vsync);
    void onKeyEvent(int key, int action);

    bool isVSynced() const;
    bool isFullscreen() const;
    GLFWwindow* getWindow() const;
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void handle_key_press(int key, int action);
    GLFWwindow* window;

    bool fullscreen;
    bool vsync;
};

#endif
