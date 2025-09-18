#include "src/lib/logger.hpp"
#include <opencv2/opencv.hpp>

int main()
{
    Logger::info("Application has started.");
    
    cv::Mat img = cv::imread("test.jpg");
    if (img.empty())
    {
        Logger::error("Image could not be read.");
        return 1;
    }
    cv::imshow("Image", img);
    cv::waitKey(0);

    Logger::info("Application reached end of it's lifecycle.");
    return 0;
}