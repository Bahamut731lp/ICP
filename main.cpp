#include "src/lib/logger.hpp"
#include "src/lib/fps_meter.hpp"
#include "src/render/render.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

#include "src/demos/lecture_3.hpp"

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

    cv::Mat frame, display_frame;
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

        if (frame.empty())
        {
            Logger::error("End of file");
            break;
        }

        get_faces_in_image(frame, face_count);

        if (face_count == 0)
        {
            display_frame = get_lock_screen_image(frame.cols, frame.rows);
        }
        else if (face_count == 1)
        {
            track_object_in_frame(frame, tracking_center);
            display_frame = frame;
        }
        else
        {
            display_frame = get_warning_screen_image(frame.cols, frame.rows);
        }

        if (FPS.is_updated())
        {
            Logger::info("FPS: " + std::to_string(FPS.get()));
        }

        cv::putText(display_frame, "FPS: " + std::to_string(static_cast<int>(FPS.get())),
                    cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0,
                    cv::Scalar(0, 255, 0), 2);

        render.draw(display_frame);
        FPS.update();

        glfwSwapBuffers(render.window);
        glfwPollEvents();
    }

    Logger::info("App has ended");
    return 0;
}
