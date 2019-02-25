#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

const std::string Run(std::string cmd);
std::vector<std::string> split(const std::string &str, char delim);
const std::string vformat(const char *const zcFormat, ...);

#endif // UTILS_H
