#include "render.hpp"
#include "logger.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <thread>

Camera* GlRender::cam = nullptr;
double window_aspect_ratio = 1.0;

std::string glStringToString(GLenum name) {
    const GLubyte* str = glGetString(name);
    return str ? std::string(reinterpret_cast<const char*>(str)) : "Unknown";
}

std::string getProfile() {
    GLint profileMask = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);

    if (profileMask & GL_CONTEXT_CORE_PROFILE_BIT) 
        return "Core Profile";
    if (profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) 
        return "Compatibility Profile";
    
    return "Unknown Profile";
}

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

void GlRender::setImguiParameters() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true); 
    ImGui_ImplOpenGL3_Init("#version 460");
}

void GlRender::setGlfwParameters() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, winWidth, winHeight, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void GlRender::setGlfwWindowInstance() {
    window = glfwCreateWindow(winWidth, winHeight, winname.c_str(), nullptr, nullptr);
    if (!window)
    {
        Logger::error("Failed to create GL window.");
        glfwTerminate();
        exit(1);
    }

    glfwSetWindowPos(window, winPos[0], winPos[1]);
    glfwMakeContextCurrent(window);
}

void GlRender::setGlfwCallbacks() {
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowUserPointer(window, this);
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

    setGlfwParameters();
    setGlfwWindowInstance();

    if (glewInit() != GLEW_OK)
    {
        Logger::error("Failed to initialize glew.");
        glfwTerminate();
        exit(1);
    }
    
    setImguiParameters();
    setGlfwCallbacks();

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
    glViewport(0, 0, frameWidth, frameHeight);

    initialized = true;
    version = glStringToString(GL_VERSION);
    profile = getProfile();
    renderer = glStringToString(GL_RENDERER);
    vendor = glStringToString(GL_VENDOR);
    shadingLanguage = glStringToString(GL_SHADING_LANGUAGE_VERSION);

    Logger::info("Version: " + version);
    Logger::info("Profile: " + profile);
    Logger::info("Render: " + renderer);
    Logger::info("Vendor: " + vendor);
    Logger::info("Shading Language: " + shadingLanguage);
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