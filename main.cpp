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

float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};
float delta = 0.0f;

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

    while (1)
    {
        glfwPollEvents();
        
        if (glfwWindowShouldClose(renderer.window))
            break;
        
        World::render(&renderer, delta);
        GUI::render(&renderer);
    
        int display_w, display_h;
        glfwGetFramebufferSize(renderer.window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw ImGui over your scene
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
