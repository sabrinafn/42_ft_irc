#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <csignal>
#include <vector>
#include <string>

/* PARSES AND VALIDATES THE PORT AND PASSWORD FROM COMMAND LINE ARGUMENTS */
std::pair<int, std::string> parsePortAndPassword(char** av);

/* SETS UP SIGNAL HANDLERS FOR SIGINT, SIGQUIT AND SIGTERM */
void setupSignals(void (*handler)(int));

/* Split a string by delimiter into non-empty tokens */
std::vector<std::string> split(const std::string& str, char delimiter);

#endif
