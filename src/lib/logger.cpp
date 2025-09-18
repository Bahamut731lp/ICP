#include "logger.hpp"

void Logger::debug(const std::string &message)
{
    Logger::log(DEBUG, message);
}

void Logger::info(const std::string &message)
{
    Logger::log(INFO, message);
}

void Logger::warning(const std::string &message)
{
    Logger::log(WARNING, message);
}

void Logger::error(const std::string &message)
{
    Logger::log(ERR, message);
}

void Logger::critical(const std::string &message)
{
    Logger::log(CRITICAL, message);
}

void Logger::log(Severity severity, const std::string &message)
{
    {
        std::time_t now = std::time(nullptr);
        std::tm tm_now;

#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_now, &now); // Windows
#else
        localtime_r(&now, &tm_now); // Linux/Unix
#endif

        std::cout << "[" << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S") << "] ";

        switch (severity)
        {
        case DEBUG:
            std::cout << "[DEBUG]";
            break;
        case INFO:
            std::cout << "[INFO]";
            break;
        case WARNING:
            std::cout << "[WARNING]";
            break;
        case ERR:
            std::cout << "[ERROR]";
            break;
        case CRITICAL:
            std::cout << "[CRITICAL]";
            break;
        }

        std::cout << "\t";
        std::cout << message;
        std::cout << std::endl;
    }
}
