#pragma once
#include "../lib/logger.hpp"
#include "../render/shapes.hpp"

cv::Mat& get_lock_screen_image(int width, int height);
cv::Mat& get_warning_screen_image(int width, int height);
cv::Mat get_faces_in_image(cv::Mat &frame, int &face_count);
void track_object_in_frame(cv::Mat &frame, cv::Point2f &center);
