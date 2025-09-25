#include "render.hpp"
#include <thread>

double window_aspect_ratio = 1.0;

void window_size_callback(GLFWwindow *window, int width, int height)
{
    int newWidth = width;
    int newHeight = newWidth / window_aspect_ratio;

    glfwSetWindowSize(window, newWidth, newHeight);
}

GlRender::GlRender()
{
}

GlRender::~GlRender()
{
    if (window)
    {
        glfwDestroyWindow(window);
    }
}

void GlRender::setScale(float s)
{
    winScale = s;
}

void GlRender::setSize(int width, int height) {
    winWidth = width;
    winHeight = height;
    frameWidth = width;
    frameHeight = height;
    glfwSetWindowSize(this->window, width, height);
}

void GlRender::init()
{
    if (window)
    {
        glfwDestroyWindow(window);
    }

    initialized = false;

    if (!glfwInit())
    {
        Logger::error("Failed to initialize glfw.");
        return;
    }

    if (atexit(glfwTerminate))
    {
        glfwTerminate();
        Logger::error("Failed to set atexit(glfwTerminate).");
        return;
    }

    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);

    window = glfwCreateWindow(winWidth, winHeight, winname.c_str(), nullptr, nullptr);

    if (!window)
    {
        Logger::error("Failed to create GL window.");
        glfwTerminate();
        exit(1);
    }

    glfwSetWindowPos(window, winPos[0], winPos[1]);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        Logger::error("Failed to initialize glew.");
        glfwTerminate();
        exit(1);
    }

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowUserPointer(window, this);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, winWidth, winHeight, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0, 0, frameWidth, frameHeight);

    initialized = true;
}

GLuint GlRender ::getTextureID(const cv::Mat &mat)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum inputFormat;
    switch (mat.channels())
    {
    case 1:
        inputFormat = GL_LUMINANCE;
        break;
    case 3:
        inputFormat = GL_BGR;
        break;
    case 4:
        inputFormat = GL_BGRA;
        break;
    }

    glTexImage2D(GL_TEXTURE_2D,    // Type of texture
                 0,                // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,           // Internal colour format to convert to
                 mat.cols,         // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,         // Image height i.e. 480 for Kinect in standard mode
                 0,                // Border width in pixels (can either be 1 or 0)
                 inputFormat,      // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE, // Image data type
                 mat.ptr());       // The actual image data itself

    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mat.cols, mat.rows, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, mat.ptr());

    return textureID;
}

void GlRender ::draw(const cv::Mat &frame)
{
    frameWidth = frame.cols;
    frameHeight = frame.rows;
    winWidth = winScale * frame.cols;
    winHeight = winScale * frame.rows;
    window_aspect_ratio = frameWidth / (double)frameHeight;

    GLuint tex = getTextureID(frame);

    glClearColor(0.1, 0.1, 0.1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(0, 0);
    glTexCoord2i(0, 1);
    glVertex2i(0, winHeight);
    glTexCoord2i(1, 1);
    glVertex2i(winWidth, winHeight);
    glTexCoord2i(1, 0);
    glVertex2i(winWidth, 0);

    glEnd();

    glDeleteTextures(1, &tex);
    glDisable(GL_TEXTURE_2D);
}

void GlRender::setFullscreen(bool fullscreen)
{
    this->fullscreen = fullscreen;
    if (fullscreen)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        glfwGetWindowPos(window, &lastWindowX, &lastWindowY);
        glfwGetWindowSize(window, &lastWindowWidth, &lastWindowHeight);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    }
    else
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window, nullptr, lastWindowX, lastWindowY, lastWindowWidth, lastWindowHeight, GLFW_DONT_CARE);
    }
}

void GlRender::setVsync(bool vsync)
{
    this->vsync = vsync;

    if (vsync)
    {
        glfwSwapInterval(1);
        return;
    }

    glfwSwapInterval(0);
}

bool GlRender::isVSynced() const
{
    return vsync;
}

bool GlRender::isFullscreen() const
{
    return fullscreen;
}

void GlRender::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GlRender* instance = static_cast<GlRender*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->onKeyEvent(key, action);
    }
}

void GlRender::onKeyEvent(int key, int action)
{
    switch (action) {
        case GLFW_PRESS:
            this->handle_key_press(key, action);
        default:
            break;
    }
}

void GlRender::handle_key_press(int key, int action)
{
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
            
            glfwTerminate();
            break;
        }
    }
}