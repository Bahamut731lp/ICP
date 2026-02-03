#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "src/lib/render.hpp"
#include "src/lib/gui.hpp"
#include "src/lib/logger.hpp"
#include "src/lib/fps_meter.hpp"
#include "src/lib/camera.hpp"
#include "src/lib/world.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>

std::thread audio_thread;
std::thread event_thread;
std::thread gui_thread;
std::thread SoundThread;

float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float delta = 0.0f;
float lastFrame = 0.0f;

int main()
{
    Logger::info("App has started");
    cv::VideoCapture capture = cv::VideoCapture(0, cv::CAP_ANY);

    Renderer::init();
    World::init();
    
    while (1)
    {
        // 1. Events
        glfwPollEvents();
        Renderer::camera->onKeyboardEvent(Renderer::window, delta);

        // 2. Clear the frame
        RenderQueue frameQueue;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        delta = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(Renderer::window, &width, &height);        
        glViewport(0, 0, width, height);

        if (glfwWindowShouldClose(Renderer::window))
            break;
        
        World::calculate(delta);

        Renderer::execute(*World::material);
        GUI::render();

        // Draw ImGui over your scene
        glfwSwapBuffers(Renderer::window);
    }

    Logger::info("App has ended");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(Renderer::window);
    glfwTerminate();
    return 0;
}
