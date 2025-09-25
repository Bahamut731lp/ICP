#include "lecture_2.hpp"

cv::CascadeClassifier face_cascade = cv::CascadeClassifier("resources/haarcascade_frontalface_default.xml");
cv::Point2f center(0.0f, 0.0f);
cv::Mat scene_cross;
cv::Mat scene_grey;

cv::Mat3b get_lightbulb_image_processed() {
    cv::Mat frame = cv::imread("resources/lightbulb.jpg");
    if (frame.empty()) {
        throw std::runtime_error("Empty file? Wrong path?");
    }

    auto start = std::chrono::steady_clock::now();
    cv::Mat frame2;
    frame.copyTo(frame2);

    // convert to grayscale, create threshold, sum white pixels
    // compute centroid of white pixels (average X,Y coordinate of all white pixels)
    cv::Point2f center;
    cv::Point2f center_normalized;
    float white_pixel_count = 0;

    for (int y = 0; y < frame.rows; y++) // y
    {
        for (int x = 0; x < frame.cols; x++) // x
        {
            // load source pixel
            cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);

            // compute temp grayscale value (convert from colors to Y)
            unsigned char Y = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 *pixel[0];

            // FIND THRESHOLD (value 0..255)
                if (Y < 240 )
                {
                    // set output pixel black
                    frame2.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                }
                else
                {
                    // set output pixel white
                    frame2.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);

                    // update centroid..
                    center.x += x;
                    center.y += y;
                    white_pixel_count += 1;
                }
        }
    }

    Logger::info(std::string("Center absolute: ") + std::to_string(center.x) + ", "+ std::to_string(center.y));
    Logger::info(std::string("Center normalized: ") + std::to_string(center_normalized.x) + ", "+ std::to_string(center_normalized.y));

    // how long the computation took?
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    Logger::debug(std::string("Elapsed time: " + std::to_string(elapsed_seconds.count()) + "sec"));

    if (white_pixel_count > 0) {
        center.x /= white_pixel_count;
        center.y /= white_pixel_count;

        center_normalized.x = center.x;
        center_normalized.y = center.y;

        center_normalized.x /= frame.cols;
        center_normalized.y /= frame.rows;
    }
    
    // highlight the center of object
    draw_cross(frame, center.x, center.y, 25);
    draw_cross_normalized(frame2, center_normalized, 25);

    // Get dimension of final image
    int rows = std::max(frame.rows, frame2.rows);
    int cols = frame.cols + frame2.cols;

    // Create a black image
    cv::Mat3b res(rows, cols, cv::Vec3b(0,0,0));

    // Copy images in correct position
    frame.copyTo(res(cv::Rect(0, 0, frame.cols, frame.rows)));
    frame2.copyTo(res(cv::Rect(frame.cols, 0, frame2.cols, frame2.rows)));

    return res;
}

cv::Mat get_faces_in_image(cv::Mat &frame)
{
	cv::cvtColor(frame, scene_grey, cv::COLOR_BGR2GRAY);
	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(scene_grey, faces);
        
	if (faces.size() > 0)
	{
        cv::rectangle(frame, faces[0], (0, 155, 155), 30);
	}

    frame.copyTo(scene_cross);
    return scene_cross;
}