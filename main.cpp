#include "src/render/render.hpp"
#include "src/lib/logger.hpp"
#include "src/lib/fps_meter.hpp"
#include "src/lib/camera.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    Logger::info("App has started");
    GlRender render;

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
    cv::Point2f tracking_center(0.0f, 0.0f);

    // display new value only once per interval (default = 1.0s)
    fps_meter FPS(1.0s);

    render.init();
    render.setScale(1);
    int face_count = 0;

    while (1)
    {
        if (glfwWindowShouldClose(render.window))
            break;

        capture.read(frame);
        if (frame.empty()) {
            continue; // Skip this loop iteration if the frame is null
            Logger::warning("Frame is empty.");
        }
        /*
            =======================
            FRAME PROCESSING START
            =======================
        */
        if (frame.empty())
        {
            Logger::error("End of file");
            break;
        }

        render.draw(frame);
        if (FPS.is_updated())
        {
            Logger::info("FPS: " + std::to_string(FPS.get()));
        }
        /*
            =======================
            FRAME PROCESSING END
            =======================
        */

        FPS.update();
        glfwSwapBuffers(render.window);
        glfwPollEvents();
    }

    Logger::info("App has ended");
    return 0;
}
