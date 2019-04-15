#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

const std::string run(std::string cmd);
std::vector<std::string> split(const std::string& str, char delim);
const std::string vformat(const char* const zcFormat, ...);

enum class ansi_color_code : int;

#endif // UTILS_H
