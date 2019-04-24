/** Generic utility functions */
#include "Utils.h"
#include <easylogging++.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <iterator>

// Run command and get text output (stdout or stderr)
//
// @param cmd Command for system to run
std::unique_ptr<result_t> run(const char* cmd)
{
    std::string data;
    FILE* stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    int rtval = -1;
    VLOG(2) << "Cmd: `" << cmd << "'";

    stream = popen(cmd, "r");
    if (stream) {
        while (fgets(buffer, max_buffer, stream) != nullptr) {
            data.append(buffer);
        }
        if (ferror(stream)) LOG(ERROR) << "error getting stream for command: '" << cmd << "'";
        rtval = pclose(stream);
        VLOG(2) << "Command return val: " << rtval;
    }
    std::unique_ptr<result_t> output = std::make_unique<result_t>();
    output->status = rtval;
    output->stdout = data;
    return output;
}

int exec(const char* file, const char* const argv[])
{
    std::size_t argc = 0;
    std::size_t len = 0;

    /* measure the inputs */
    for (auto* p = argv; *p; ++p) {
        VLOG(3) << "Arg " << argc << ": " << argv[argc];
        ++argc;
        len += std::strlen(*p) + 1;
    }
    /* allocate copies */
    auto const arg_string = std::make_unique<char[]>(len);
    auto const args = std::make_unique<char*[]>(argc + 1);
    /* copy the inputs */
    len = 0; // re-use for position in arg_string
    for (auto i = 0u; i < argc; ++i) {
        len += std::strlen(args[i] = std::strcpy(&arg_string[len], argv[i])) +
               1; /* advance to one AFTER the nul */
    }
    args[argc] = nullptr;
    return execvp(file, args.get());
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
std::vector<std::string_view> split(const std::string_view str, std::string_view delims = " ")
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

// Read first line of file into string
// @param filename Path of file to read
const std::string read_first_line(const char* filename)
{
    std::ifstream file;
    std::string line;
    file.open(filename);

    if (!file) {
        std::cerr << Ansi::setFg(Color::brred) << "Error opening file: " << Ansi::reset()
                  << filename << std::endl;
        LOG(ERROR) << "Could not open file: " << filename;
    } else {
        std::getline(file, line);
    }
    return line;
}
