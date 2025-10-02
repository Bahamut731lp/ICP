#include "lecture_3.hpp"

static cv::CascadeClassifier face_cascade = cv::CascadeClassifier("resources/haarcascade_frontalface_default.xml");

cv::Mat scene_grey2;
cv::Mat small_frame;
static cv::Mat lock_screen_cache;
static cv::Mat warning_screen_cache;

cv::Mat scene_cross2;

cv::Mat &get_lock_screen_image(int width, int height)
{
    if (lock_screen_cache.empty() || lock_screen_cache.cols != width || lock_screen_cache.rows != height)
    {
        cv::Mat img = cv::imread("resources/lightbulb.jpg");
        if (!img.empty())
        {
            cv::resize(img, lock_screen_cache, cv::Size(width, height));
        }
        else
        {
            lock_screen_cache = cv::Mat(height, width, CV_8UC3, cv::Scalar(20, 20, 80));
            cv::putText(lock_screen_cache, "SCREEN LOCKED", cv::Point(width / 4, height / 2),
                        cv::FONT_HERSHEY_TRIPLEX, 1.5, cv::Scalar(255, 255, 255), 3);
            cv::putText(lock_screen_cache, "No face detected", cv::Point(width / 3, height / 2 + 50),
                        cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(200, 200, 200), 2);
        }
    }
    return lock_screen_cache;
}

cv::Mat &get_warning_screen_image(int width, int height)
{
    if (warning_screen_cache.empty() || warning_screen_cache.cols != width || warning_screen_cache.rows != height)
    {
        warning_screen_cache = cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 100));
        cv::putText(warning_screen_cache, "WARNING!", cv::Point(width / 3, height / 3),
                    cv::FONT_HERSHEY_TRIPLEX, 2.0, cv::Scalar(0, 0, 255), 4);
        cv::putText(warning_screen_cache, "Multiple faces detected!", cv::Point(width / 6, height / 2),
                    cv::FONT_HERSHEY_TRIPLEX, 1.2, cv::Scalar(255, 255, 255), 3);
        cv::putText(warning_screen_cache, "Someone may be watching", cv::Point(width / 6, height / 2 + 50),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(200, 200, 200), 2);
    }
    return warning_screen_cache;
}

void track_object_in_frame(cv::Mat &frame, cv::Point2f &center)
{
    cv::Mat grey;
    cv::cvtColor(frame, grey, cv::COLOR_BGR2GRAY);

    cv::Mat threshold_img;
    cv::threshold(grey, threshold_img, 200, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(threshold_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (!contours.empty())
    {
        size_t largest_idx = 0;
        double largest_area = 0;

        for (size_t i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            if (area > largest_area)
            {
                largest_area = area;
                largest_idx = i;
            }
        }

        if (largest_area > 100)
        {
            cv::Moments m = cv::moments(contours[largest_idx]);
            if (m.m00 > 0)
            {
                center.x = m.m10 / m.m00;
                center.y = m.m01 / m.m00;

                draw_cross(frame, center.x, center.y, 30);
                cv::circle(frame, center, 5, cv::Scalar(0, 255, 0), -1);
                cv::drawContours(frame, contours, largest_idx, cv::Scalar(0, 255, 255), 2);
            }
        }
    }
}
cv::Mat get_faces_in_image(cv::Mat &frame, int &face_count)
{

    cv::Mat small_frame;
    double scale = 0.5;
    cv::resize(frame, small_frame, cv::Size(), scale, scale, cv::INTER_LINEAR);

    cv::cvtColor(small_frame, scene_grey2, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(scene_grey2, scene_grey2);

    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(scene_grey2, faces, 1.2, 3, 0);

    face_count = faces.size();
    if (face_count > 0)
    {
        cv::Rect scaled_face(
            faces[0].x / scale,
            faces[0].y / scale,
            faces[0].width / scale,
            faces[0].height / scale);
        cv::rectangle(frame, scaled_face, cv::Scalar(0, 155, 255), 5);
    }

    frame.copyTo(scene_cross2);
    return scene_cross2;
}