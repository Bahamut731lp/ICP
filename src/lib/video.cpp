#include "video.hpp"
#include "logger.hpp"


std::thread Video::worker;
std::mutex Video::mutex;
std::atomic<bool> Video::is_running{false};
cv::Mat Video::frame;
bool Video::is_frame_ready = false;
cv::VideoCapture Video::capture; // Standard object initialization
GLuint Video::textureId = 0;
cv::Ptr<cv::FaceDetectorYN> Video::detector; 
void Video::init()
{
    if (is_running)
        return;

    is_running = true;

    Video::detector = cv::FaceDetectorYN::create(
        "resources/face_detection_yunet_2023mar.onnx",
        "",
        cv::Size(640, 480),
        0.9f,
        0.3f,
        5000
    );
    
    // Inside your Video initialization logic:
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // These parameters are MANDATORY. Without them, the texture often defaults to black.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Pre-allocate the memory (use your camera's resolution)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    
    capture = cv::VideoCapture(0, cv::CAP_ANY);
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    capture.open(0);

    worker = std::thread(&Video::loop);

    Logger::info("VideoService: Thread started.");
}

void Video::shutdown()
{
    is_running = false;
    
    if (worker.joinable())
    {
        worker.join();
    }
    
    Logger::info("VideoService: Thread joined.");
}

void Video::loop()
{
    while (is_running)
    {
        cv::Mat current_frame;
        bool is_ok = capture.read(current_frame);
        
        if (!is_ok) {
            continue;
        }

        cv::Mat faces;
        detector->setInputSize(current_frame.size());
        detector->detect(current_frame, faces);

        for (int i = 0; i < faces.rows; i++) {
            float x = faces.at<float>(i, 0);
            float y = faces.at<float>(i, 1);
            float w = faces.at<float>(i, 2);
            float h = faces.at<float>(i, 3);
            float score = faces.at<float>(i, 14);

            cv::Scalar color = cv::Scalar(0, 255, 0); // Green
            cv::rectangle(current_frame, cv::Rect(int(x), int(y), int(w), int(h)), color, 2);

            std::string label = cv::format("Face: %.2f", score);
            cv::putText(current_frame, label, cv::Point(int(x), int(y) - 10), 
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
        }

        cv::cvtColor(current_frame, current_frame, cv::COLOR_BGR2RGB);

        std::lock_guard<std::mutex> lock(mutex);
        current_frame.copyTo(frame);
        is_frame_ready = true;

    }   
}

bool Video::getFrame(cv::Mat& output) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!is_frame_ready) return false;
    
    frame.copyTo(output);
    is_frame_ready = false;
    return true;
}

int Video::getTextureId() {
    return textureId;
}