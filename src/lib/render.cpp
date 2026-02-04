#include "render.hpp"
#include "logger.hpp"
#include "audio.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <thread>

Camera *Renderer::camera = nullptr;
GLFWwindow *Renderer::window = nullptr;
CursorMode Renderer::cursor = LOCKED;
bool Renderer::antialiased = false;
bool Renderer::maximized = false;
bool Renderer::vsync = false;
bool Renderer::fullscreen = false;
bool Renderer::isMouseMoved = false;

int Renderer::lastWindowX = 0;
int Renderer::lastWindowY = 0;
int Renderer::lastWindowWidth = 0;
int Renderer::lastWindowHeight = 0;
int Renderer::winWidth = 1280;
int Renderer::winHeight = 720;

std::string Renderer::version = "";
std::string Renderer::profile = "";
std::string Renderer::renderer = "";
std::string Renderer::vendor = "";
std::string Renderer::shadingLanguage = "";

float Renderer::winScale = 0.5;
float Renderer::lastX = 0.0f;
float Renderer::lastY = 0.0f;

RenderQueue Renderer::queue;

std::array<int, 2> Renderer::position = {0, 0};
std::string Renderer::name = "ICP";

double window_aspect_ratio = 1.0;

std::string glStringToString(GLenum name)
{
    const GLubyte *str = glGetString(name);
    return str ? std::string(reinterpret_cast<const char *>(str)) : "Unknown";
}

std::string getProfile()
{
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

void window_maximize_callback(GLFWwindow *window, int maximized)
{
    if (maximized)
    {
        Logger::info("Window was maximized");
    }
    else
    {
        Logger::info("Window was restored");
    }

    Renderer *instance = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
    if (instance)
    {
        Logger::info("Setting stuff in instance");
        instance->setMaximization((bool)maximized);
    }
}

void Renderer::setCursor(CursorMode cursor)
{
    Renderer::cursor = cursor;

    switch (cursor)
    {
    case FREE:
        glfwSetInputMode(Renderer::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case LOCKED:
        glfwSetInputMode(Renderer::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    }
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    if (window)
    {
        glfwDestroyWindow(window);
    }
}

void Renderer::setScale(float s)
{
    winScale = s;
}

int Renderer::getWidth()
{
    return Renderer::winWidth;
}

int Renderer::getHeight()
{
    return winHeight;
}

void Renderer::setSize(int width, int height)
{
    winWidth = width;
    winHeight = height;
    glfwSetWindowSize(window, width, height);
}

void Renderer::setImguiParameters()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Renderer::setWindowHints()
{
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

void Renderer::setGlfwFeatures()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::setGlfwWindowInstance()
{
    window = glfwCreateWindow(winWidth, winHeight, name.c_str(), nullptr, nullptr);
    if (!window)
    {
        Logger::error("Failed to create GL window.");
        glfwTerminate();
        exit(1);
    }

    glfwSetWindowPos(window, position[0], position[1]);
    glfwMakeContextCurrent(window);
}

void Renderer::setGlfwCallbacks()
{
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowMaximizeCallback(window, window_maximize_callback);
    glfwSetCursorPosCallback(window, Renderer::mouse_callback);
    glfwSetKeyCallback(window, Renderer::key_callback);
}

void Renderer::init()
{
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

    Renderer::setCursor(LOCKED);
}

GLuint Renderer ::getTextureID(const cv::Mat &mat)
{
    GLuint textureID;

    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum inputFormat;
    GLenum internalFormat = GL_RGBA8; // Standard internal format

    switch (mat.channels())
    {
    case 1:
        inputFormat = GL_RED; // DSA preferred over GL_LUMINANCE
        internalFormat = GL_R8;
        break;
    case 3:
        inputFormat = GL_BGR;
        internalFormat = GL_RGB8;
        break;
    case 4:
        inputFormat = GL_BGRA;
        internalFormat = GL_RGBA8;
        break;
    }

    glTextureStorage2D(textureID, 1, internalFormat, mat.cols, mat.rows);
    glTextureSubImage2D(textureID, 0, 0, 0, mat.cols, mat.rows, inputFormat, GL_UNSIGNED_BYTE, mat.ptr());

    return textureID;
}

void Renderer::setFullscreen(bool fullscreen)
{
    Logger::info("Fullscreen:\t" + std::string(fullscreen ? "enabled" : "disabled"));
    Renderer::fullscreen = fullscreen;
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

void Renderer::setVsync(bool vsync)
{
    Logger::info("V-Sync:\t" + std::string(vsync ? "enabled" : "disabled"));
    glfwMakeContextCurrent(window);

    Renderer::vsync = vsync;

    if (vsync)
    {
        glfwSwapInterval(1);
        return;
    }

    glfwSwapInterval(0);
}

void Renderer::setMaximization(bool maximized)
{
    Logger::info("Maximization:\t" + std::string(maximized ? "enabled" : "disabled"));
    Renderer::maximized = maximized;

    if (maximized)
    {
        glfwMaximizeWindow(window);
    }
    else
    {
        glfwRestoreWindow(window);
    }
}

void Renderer::setAntialiasing(bool antialised)
{
    Logger::info("Antialising:\t" + std::string(antialised ? "enabled" : "disabled"));
    Renderer::antialiased = antialiased;

    if (antialiased)
    {
        glEnable(GL_MULTISAMPLE);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);
    }
}

bool Renderer::isVSynced()
{
    return vsync;
}

bool Renderer::isFullscreen()
{
    return fullscreen;
}

bool Renderer::isMaximized()
{
    return maximized;
}

bool Renderer::isAntialiased()
{
    return antialiased;
}

void Renderer::getScreenshot()
{
    cv::Mat pixels(winHeight, winWidth, CV_8UC3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, winWidth, winHeight, GL_BGR, GL_UNSIGNED_BYTE, pixels.data);

    cv::Mat flipped;
    cv::flip(pixels, flipped, 0);

    // 5. Uložíme pomocí OpenCV
    if (cv::imwrite("screenshot.png", flipped))
    {
        Logger::info("Screenshot saved to: screenshot.png");
    }
    else
    {
        Logger::error("Failed to save screenshot!");
    }
}

void Renderer::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Handle Left Click
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        Logger::info("Left Click at: " + std::to_string(xpos) + ", " + std::to_string(ypos));
    }
}

void Renderer::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_V) {
            Renderer::setCursor(FREE);
            Audio::play("resources/audio/click.mp3");
        }
        if (key == GLFW_KEY_C) {
            Renderer::setCursor(LOCKED);
            Audio::play("resources/audio/click.mp3");
        }
    }
}

void Renderer::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    // Check if any camera was assigned to the window,
    // since default value is null pointer, which could lead
    // to crashes. We do not want that.
    if (Renderer::camera == nullptr)
    {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (Renderer::isMouseMoved)
    {
        Renderer::lastX = xpos;
        Renderer::lastY = ypos;
        Renderer::isMouseMoved = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    Renderer::camera->onMouseEvent(xoffset, yoffset, GL_TRUE);
}

void Renderer::draw(const RenderCommand &cmd, Shader &shader)
{
    shader.activate();

    float aspect = (float)Renderer::winWidth / (float)Renderer::winHeight;
    shader.setUniform("transform", cmd.transform);
    shader.setUniform("view", (*Renderer::camera).getViewMatrix());
    shader.setUniform("projection", (*Renderer::camera).getProjectionMatrix(aspect));

    // Set material uniforms from the mesh
    shader.setUniform("material.diffuse", cmd.mesh->material.diffuse);
    shader.setUniform("material.specular", cmd.mesh->material.specular);
    shader.setUniform("material.shininess", cmd.mesh->material.shininess);
    shader.setUniform("material.transparency", cmd.mesh->material.transparency);

    // Texture Logic
    if (cmd.mesh->material.texture.id != -1)
    {
        shader.setUniform("material.texture.textureUnit", 0);
        shader.setUniform("material.texture.isTextured", 1);
        shader.setUniform("material.texture.scale", cmd.mesh->material.texture.scale);

        glBindTextureUnit(0, cmd.mesh->material.texture.id);
    }
    else
    {
        shader.setUniform("material.texture.isTextured", 0);
        glBindTextureUnit(0, 0);
    }

    cmd.mesh->draw(shader);
}

void Renderer::execute(Shader &shader)
{
    // Update Audio
    Audio::updateListener(camera->Position, camera->Front);

    std::sort(queue.opaque.begin(), queue.opaque.end(), [](const RenderCommand &a, const RenderCommand &b)
              { return a.distance < b.distance; });

    std::sort(queue.transparent.begin(), queue.transparent.end(), [](const RenderCommand &a, const RenderCommand &b)
              { return a.distance > b.distance; });

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    for (const auto &cmd : queue.opaque)
    {
        Renderer::draw(cmd, shader);
    }

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    for (const auto &cmd : queue.transparent)
    {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        Renderer::draw(cmd, shader);

        glCullFace(GL_BACK);
        glDisable(GL_POLYGON_OFFSET_FILL);
        Renderer::draw(cmd, shader);
    }

    glDepthMask(GL_TRUE);
    queue.clear();
}