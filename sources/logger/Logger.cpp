#include "../includes/ft_irc.hpp"

void logInfo(const std::string& msg) {
    std::cout << GREEN << "[INFO] " << RST << msg << std::endl;
}

void logDebug(const std::string& msg) {
    std::cout << BLUE << "[DEBUG] " << RST << msg << std::endl;
}

void logError(const std::string& msg) {
    std::cout << RED << "[ERROR] " << RST << msg << std::endl;
}
