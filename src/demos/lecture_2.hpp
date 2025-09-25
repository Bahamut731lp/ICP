#pragma once
#include "../lib/logger.hpp"
#include "../render/shapes.hpp"

cv::Mat3b get_lightbulb_image_processed();
cv::Mat get_faces_in_image(cv::Mat &frame);