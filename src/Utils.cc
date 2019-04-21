/** Generic utility functions */
#include "Utils.h"
#include <easylogging++.h>
#include <iostream>
#include <sstream>


// Run command and get text output (stdout or stderr)
//
// @param cmd Command for system to run
const std::string run(const char* cmd)
{
    std::string data;
    FILE* stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    VLOG(2) << "Cmd: `" << cmd << "'";

    stream = popen(cmd, "r");
    if (stream) {
        while (fgets(buffer, max_buffer, stream) != nullptr) {
            data.append(buffer);
        }
        if (ferror(stream)) LOG(ERROR) << "error getting stream for command: '" << cmd << "'";
        pclose(stream);
    }
    return data;
}

// Split string by delimiter
//
// @param str String to split
// @param delim Character to use as delimter
std::vector<std::string> split(const std::string& str, char delim = ' ')
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Split string view by one or more delimiters
// @param str String view to split
// @param delims Delimiter(s)
std::vector<std::string_view> splitSV(std::string_view str, std::string_view delims = " ")
{
    std::vector<std::string_view> output;
    for (auto first = str.data(), second = str.data(), last = first + str.size();
         second != last && first != last; first = second + 1) {
        second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));
        if (first != second) output.emplace_back(first, second - first);
    }
    return output;
}

namespace Ansi {
    // Set foreground color
    //
    // @param color Color from Ansi::Color enum
    std::string setFg(Color color)
    {
        const char* env_term = getenv("TERM");
        if (env_term == nullptr || strcmp(env_term, "dumb") == 0) {
            return "";
        }
        std::stringstream escape_fg;
        escape_fg << "\033[38;5;" << static_cast<unsigned int>(color) << "m";
        return escape_fg.str();
    }

    // Set background color
    //
    // @param color Color from Ansi::Color enum
    std::string setBg(Color color)
    {
        const char* env_term = getenv("TERM");
        if (env_term == nullptr || strcmp(env_term, "dumb") == 0) {
            return "";
        }
        std::stringstream escape_fg;
        escape_fg << "\033[48;5;" << static_cast<unsigned int>(color) << "m";
        return escape_fg.str();
    }

    // Reset colors
    std::string reset()
    {
        const char* env_term = getenv("TERM");
        if (env_term == nullptr || strcmp(env_term, "dumb") == 0) {
            return "";
        }
        return "\033[0m";
    }
} // namespace Ansi
