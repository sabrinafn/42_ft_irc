#ifndef LOGGER_HPP
#define LOGGER_HPP

#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"
#define RST     "\033[0m"

void logInfo(const std::string& msg);
void logDebug(const std::string& msg);
void logError(const std::string& msg);


#endif
