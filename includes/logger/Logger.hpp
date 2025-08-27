#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

class Logger {
   public:
    Logger();
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);
    ~Logger();

    void info(const std::string& msg);
    void debug(const std::string& msg);
    void error(const std::string& msg);

   private:
    std::string getCurrentTime();
};

#endif
