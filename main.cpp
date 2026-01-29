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

float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float delta = 0.0f;
float lastFrame = 0.0f;

int main()
{
    Logger::info("App has started");
    GlRender renderer;

    cv::VideoCapture capture = cv::VideoCapture(0, cv::CAP_ANY);

    if (!capture.isOpened())
    {
        Logger::error("Camera cannot be open.");
        return 1;
    }
    else
    {
        Logger::info("Source: Width: " + std::to_string(capture.get(cv::CAP_PROP_FRAME_WIDTH)) + "; Height: " + std::to_string(capture.get(cv::CAP_PROP_FRAME_HEIGHT)));
    }

    cv::Mat frame;
    renderer.init();
    renderer.setScale(1);

    World::init();
    glfwSetInputMode(renderer.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    while (1)
    {
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        delta = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(renderer.window, &width, &height);        
        glViewport(0, 0, width, height);

        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (glfwWindowShouldClose(renderer.window))
            break;
        
        World::render(&renderer, delta);
	    GlRender::cam->onKeyboardEvent(renderer.window, delta);
        
        GUI::render(&renderer);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Draw ImGui over your scene
        glfwSwapBuffers(renderer.window);
    }

    Logger::info("App has ended");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(renderer.window);
    glfwTerminate();
    return 0;
}
