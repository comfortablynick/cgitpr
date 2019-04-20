// Generic utility functions
#include "Utils.h"
#include <easylogging++.h>
#include <iostream>
#include <sstream>


// Run command and get text output (stdout or stderr)
// @param cmd Command for system to run
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

// Split string by delimiter
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
namespace Ansi {
    // Set foreground color
    // @param color Color from Ansi::Fg enum
    std::string setFg(Fg color)
    {
        const char* env_term = getenv("TERM");
        if (env_term == nullptr || strcmp(env_term, "dumb") == 0) {
            return "";
        }
        if (color == Fg::reset) {
            return "\033[0m";
        }
        std::stringstream escape_fg;
        escape_fg << "\033[38;5;" << static_cast<unsigned int>(color) << "m";
        return escape_fg.str();
    }
} // namespace Ansi
