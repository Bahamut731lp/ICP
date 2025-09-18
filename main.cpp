#include "src/lib/logger.hpp"
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat img = cv::imread("test.jpg");
    if (img.empty())
    {
        std::cout << "Could not read the image\n";
        return 1;
    }
    cv::imshow("Image", img);
    cv::waitKey(0);
    Logger::info("New Application has been constructed.");
    return 0;
}