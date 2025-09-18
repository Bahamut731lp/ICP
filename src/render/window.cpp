#include "window.hpp"

Window::Window(int width, int height, const char* title, bool fullscreen, bool vsync)
    : fullscreen(fullscreen), vsync(vsync)
{
    this->width = width;
    this->height = height;


    if (!glfwInit()) {
        Logger::error("Failed to initialize GLFW");
        std::exit(-1);
    }

    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);
    }
    else {
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    }

    if (!window) {
        Logger::error("Failed to create GLFW window");
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Window::getWindow() const {
    return window;
}
