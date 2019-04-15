// Generic utility functions
#include "Utils.h"
#include <cstdarg>
#include <easylogging++.h>
#include <iostream>
#include <sstream>

/* Run command and get text output (stdout or stderr) */
const std::string run(std::string cmd)
{
    std::string data;
    FILE* stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    VLOG(2) << "Cmd: `" << cmd << "'";

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (fgets(buffer, max_buffer, stream) != nullptr) {
            data.append(buffer);
        }
        if (ferror(stream)) LOG(ERROR) << "error getting stream for command: '" << cmd << "'";
        pclose(stream);
    }
    return data;
}

/* Split string by delimiter */
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

const std::string vformat(const char* const zcFormat, ...)
{

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), iLen);
}

enum class ansi_color_code : int
{
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    white = 37,
    bright_black = 90,
    bright_red = 91,
    bright_green = 92,
    bright_yellow = 93,
    bright_blue = 94,
    bright_magenta = 95,
    bright_cyan = 96,
    bright_white = 97,
};

template <typename printable>
std::string print_as_color(printable const& value, ansi_color_code color)
{
    std::stringstream sstr;
    sstr << "\033[1;" << static_cast<int>(color) << "m" << value << "\033[0m";
    return sstr.str();
}
template <ansi_color_code color, typename printable>
std::string print_as_color(printable const& value)
{
    std::stringstream sstr;
    sstr << "\033[1;" << static_cast<int>(color) << "m" << value << "\033[0m";
    return sstr.str();
}
