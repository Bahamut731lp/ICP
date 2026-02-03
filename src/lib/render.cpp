#include "render.hpp"
#include "logger.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <thread>

Camera* GlRender::cam = nullptr;
float GlRender::lastX = 0.0f;
float GlRender::lastY = 0.0f;
bool GlRender::isMouseMoved = false;

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
}

void window_maximize_callback(GLFWwindow* window, int maximized) {
    if (maximized) {
        Logger::info("Window was maximized");
    } else {
        Logger::info("Window was restored");
    }
    
    GlRender* instance = static_cast<GlRender*>(glfwGetWindowUserPointer(window));
    if (instance) {
        Logger::info("Setting stuff in instance");
        instance->setMaximization((bool)maximized);
    }
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

int GlRender::getWidth() const
{
    return winWidth; 
}

int GlRender::getHeight() const
{
    return winHeight; 
}

void GlRender::setSize(int width, int height) {
    winWidth = width;
    winHeight = height;
    glfwSetWindowSize(this->window, width, height);
}

void GlRender::setImguiParameters() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true); 
    ImGui_ImplOpenGL3_Init("#version 410");
}

void GlRender::setWindowHints() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 1. Must be Core Profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 2. Required for macOS
}

void GlRender::setGlfwFeatures() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowMaximizeCallback(window, window_maximize_callback);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, GlRender::mouse_callback);
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

    setWindowHints();
    setGlfwWindowInstance();

    if (glewInit() != GLEW_OK)
    {
        Logger::error("Failed to initialize glew.");
        glfwTerminate();
        exit(1);
    }
    
    setGlfwFeatures();
    setGlfwCallbacks();
    setImguiParameters();

    glfwGetFramebufferSize(window, &winWidth, &winHeight);
    glViewport(0, 0, winWidth, winHeight);

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

void GlRender::setFullscreen(bool fullscreen)
{
    Logger::info("Fullscreen:\t" + std::string(fullscreen ? "enabled" : "disabled"));
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
    Logger::info("V-Sync:\t" + std::string(vsync ? "enabled" : "disabled"));
    glfwMakeContextCurrent(window);

    this->vsync = vsync;

    if (vsync)
    {
        glfwSwapInterval(1);
        return;
    }

    glfwSwapInterval(0);
}

void GlRender::setMaximization(bool maximized)
{
    Logger::info("Maximization:\t" + std::string(maximized ? "enabled" : "disabled"));
    this->maximized = maximized;

    if (maximized) {
        glfwMaximizeWindow(window);
    } else {
        glfwRestoreWindow(window);
    }
}

void GlRender::setAntialiasing(bool antialised)
{
    Logger::info("Antialising:\t" + std::string(antialised ? "enabled" : "disabled"));
    this->antialiased = antialiased;

    if (antialiased) {
        glEnable(GL_MULTISAMPLE);
    } else {
        glDisable(GL_MULTISAMPLE);
    }
}

bool GlRender::isVSynced() const
{
    return vsync;
}

bool GlRender::isFullscreen() const
{
    return fullscreen;
}

bool GlRender::isMaximized() const
{
    return maximized;
}

bool GlRender::isAntialiased() const
{
    return antialiased;
}

void GlRender::getScreenshot() const
{
    cv::Mat pixels(winHeight, winWidth, CV_8UC3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, winWidth, winHeight, GL_BGR, GL_UNSIGNED_BYTE, pixels.data);

    cv::Mat flipped;
    cv::flip(pixels, flipped, 0);

    // 5. Uložíme pomocí OpenCV
    if (cv::imwrite("screenshot.png", flipped)) {
        Logger::info("Screenshot saved to: screenshot.png");
    } else {
        Logger::error("Failed to save screenshot!");
    }
}

void GlRender::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GlRender* instance = static_cast<GlRender*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->onKeyEvent(key, action);
    }
}

void GlRender::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Check if any camera was assigned to the window,
    // since default value is null pointer, which could lead
    // to crashes. We do not want that.
    if (GlRender::cam == nullptr) {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (GlRender::isMouseMoved)
    {
        GlRender::lastX = xpos;
        GlRender::lastY = ypos;
        GlRender::isMouseMoved = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    GlRender::cam->onMouseEvent(xoffset, yoffset, GL_TRUE);
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