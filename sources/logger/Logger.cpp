#include "../includes/ft_irc.hpp"

Logger::Logger() {
}

Logger::Logger(const Logger& other) {
    (void)other;
}

Logger& Logger::operator=(const Logger& other) {
    (void)other;
    return *this;
}

Logger::~Logger() {
}

void Logger::debug(const std::string& msg) {
    std::cout << "[" << getCurrentTime() << "] [DEBUG] " << msg << std::endl;
}

void Logger::info(const std::string& msg) {
    std::cout << "[" << getCurrentTime() << "] [INFO]  " << msg << std::endl;
}

void Logger::error(const std::string& msg) {
    std::cerr << "[" << getCurrentTime() << "] [ERROR] " << msg << std::endl;
}

std::string Logger::getCurrentTime() {
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    std::stringstream ss;
    ss << (now->tm_hour < 10 ? "0" : "") << now->tm_hour << ":"
       << (now->tm_min < 10 ? "0" : "") << now->tm_min << ":"
       << (now->tm_sec < 10 ? "0" : "") << now->tm_sec;
    return ss.str();
}