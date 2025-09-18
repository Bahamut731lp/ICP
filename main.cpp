#include "src/lib/logger.hpp"
#include "src/render/window.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    Logger::info("App has started");
    Window* window = new Window(1280, 720, "ICP");
    
    // Main loop
    while (!glfwWindowShouldClose(window->getWindow()))
    {
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers and poll events
        glfwSwapBuffers(window->getWindow());
        glfwPollEvents();
    }

    glfwTerminate();
    Logger::info("App has ended");
    return 0;
}
