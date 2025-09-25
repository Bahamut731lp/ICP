#include "src/lib/logger.hpp"
#include "src/render/render.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

#include "src/demos/lecture_2.hpp"

int main()
{
    Logger::info("App has started");
    GlRender render;

    cv::Mat3b image = get_lightbulb_image_processed();
    cv::VideoCapture capture = cv::VideoCapture(0, cv::CAP_ANY);

    if (!capture.isOpened())
    { 
        Logger::error("Camera cannot be open.");
        return false;
    }
    else
    {
        Logger::info("Source: Width: " + std::to_string(capture.get(cv::CAP_PROP_FRAME_WIDTH)) + "; Height: " + std::to_string(capture.get(cv::CAP_PROP_FRAME_HEIGHT)));
    }

    cv::Mat frame, scene;
    cv::CascadeClassifier face_cascade = cv::CascadeClassifier("resources/haarcascade_frontalface_default.xml");
    
    render.init();
    render.setScale(1);

    while (1)
    {
        // how long the computation took?

        if (glfwWindowShouldClose(render.window)) break;
        auto start = std::chrono::steady_clock::now();

        capture.read(frame);

        if (frame.empty())
        {
            Logger::error("End of file");
            break;
        }

        render.draw(get_faces_in_image(frame));
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        Logger::debug(std::string("Elapsed time: " + std::to_string(elapsed_seconds.count()) + "sec"));
        
        glfwSwapBuffers(render.window);
        glfwPollEvents();

        if (cv::waitKey(1) == 27)
            break;
    }

    Logger::info("App has ended");
    return 0;
}
