#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <csignal>

/* PARSES AND VALIDATES THE PORT AND PASSWORD FROM COMMAND LINE ARGUMENTS */
std::pair<int, std::string> parsePortAndPassword(char **av);

/* SETS UP SIGNAL HANDLERS FOR SIGINT, SIGQUIT AND SIGTERM */
void setupSignals(void (*handler)(int));

#endif
