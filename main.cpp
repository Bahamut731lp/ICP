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
    render.start(image);
    
    Logger::info("App has ended");
    return 0;
}
