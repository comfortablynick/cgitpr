#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

const std::string run(std::string cmd);
std::vector<std::string> split(const std::string& str, char delim);
const std::string vformat(const char* const zcFormat, ...);

enum class ansi_color_code : int;

// template <typename printable>
// std::string print_as_color(printable const& value, ansi_color_code color);
//
// template <ansi_color_code color, typename printable>
// std::string print_as_color(printable const& value);

#endif // UTILS_H
