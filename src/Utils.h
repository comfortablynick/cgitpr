#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <string>
#include <vector>

const std::string run(std::string cmd);
std::vector<std::string> split(const std::string& str, char delim);

namespace Ansi {
    enum class Fg : unsigned int
    {
        reset = 0,
        green = 34,
        cyan = 37,
        yellow = 142,
        brcyan = 51,
        bryellow = 226,
    };

    // base colors
    char const* const reset = "\033[0m";
    namespace fg {
        // foreground colors
        char const* const green = "\033[38;5;34m";
        char const* const cyan = "\033[38;5;37m";
        char const* const yellow = "\033[38;5;142m";
    } // namespace fg
    namespace fgB {
        // foreground bright colors
        char const* const cyan = "\033[38;5;51m";
        char const* const yellow = "\033[38;5;226m";
    } // namespace fgB
    std::string setFg(Fg);
} // namespace Ansi


#endif // UTILS_H
