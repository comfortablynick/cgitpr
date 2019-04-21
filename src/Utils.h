#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <string>
#include <vector>

const std::string run(const char*);
std::vector<std::string> split(const std::string&, char);
std::vector<std::string_view> splitSV(std::string_view, std::string_view);

enum class Color : unsigned int
{
    black = 0,
    blue = 12,
    green = 2,
    cyan = 37,
    red = 124,
    yellow = 142,
    brcyan = 51,
    bryellow = 226,
};
namespace Ansi {
    std::string reset(void);
    std::string setFg(Color);
    std::string setBg(Color);
} // namespace Ansi
#endif // UTILS_H
