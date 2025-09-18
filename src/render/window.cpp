#include "window.hpp"

Window::Window(int width, int height, const char* title, bool fullscreen, bool vsync)
    : fullscreen(fullscreen), vsync(vsync)
{
    Logger::debug("Creating a new window");
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
}

Window::~Window() {
    Logger::debug("Destroying window");
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* Window::getWindow() const {
    return window;
}

void Window::setFullscreen(bool fullscreen) {
    this->fullscreen = fullscreen;
    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwGetWindowPos(window, &lastWindowX, &lastWindowY);
        glfwGetWindowSize(window, &lastWindowWidth, &lastWindowHeight);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    }
    else {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window, nullptr, lastWindowX, lastWindowY, lastWindowWidth, lastWindowHeight, GLFW_DONT_CARE);
    }
}

void Window::setVsync(bool vsync)
{
    this->vsync = vsync;

    if (vsync) {
        glfwSwapInterval(1);
        return;
    }

    glfwSwapInterval(0);
}

bool Window::isVSynced() const
{
    return vsync;
}

bool Window::isFullscreen() const {
    return fullscreen;
}

// Static key callback function
void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // glfwGetWindowUserPointer() vyt�hne z okna custom pointer, kter� je k n�mu p�i�azen
    // v na�em p��pad� jsem k tomu p�idadil instanci t��dy Window v jej�m konstruktoru
    // static_cast<Window*> je type_casting syntax (p�evod datov�ho typu) - t�m my c++ �ekneme, �e ten pointer je na t��du Window
    Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->onKeyEvent(key, action);
    }
}

/*
    Funkce pro handlov�n� inputu v okn�.
*/
void Window::onKeyEvent(int key, int action) {

    switch (action) {
        case GLFW_PRESS:
            this->handle_key_press(key, action);
        default:
            break;
    }
}

void Window::handle_key_press(int key, int action) {
    switch (key) {
        case GLFW_KEY_F: {
            Logger::info("Fullscreen:\t" + std::string(fullscreen ? "enabled" : "disabled"));
            
            fullscreen = !fullscreen;
            setFullscreen(fullscreen);
            break;
        }
        case GLFW_KEY_V: {
            Logger::info("V-Sync:\t" + std::string(vsync ? "enabled" : "disabled"));
            
            vsync = !vsync;
            setVsync(vsync);
            break;
        }
        case GLFW_KEY_ESCAPE: {
            Logger::info("Closing window through key shortcut.");
            
            this->~Window();
            break;
        }
    }
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Window* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->width = width;
        instance->height = height;
    }
}
