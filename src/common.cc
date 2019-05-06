/** Common code for project */
#include "common.h"
#include <algorithm>
#include "loguru.hpp"
#include <errno.h>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// String representation of (null-terminated) const char* array.
//
// @param argv Array of char pointers
std::string
prettify(const char* argv[])
{
    std::ostringstream out;
    out << '{';

    for (size_t i = 0; argv[i]; ++i) {
        if (i != 0) out << ", ";
        const char* ch = argv[i];
        while (*ch) out << *(ch++);
    }

    out << '}';
    return out.str();
}

// Run command and get text output (stdout or stderr)
//
// @param cmd Command for system to run
std::unique_ptr<result_t>
run(const char* cmd)
{
    std::string data;
    FILE* stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    int rtval = -1;
    LOG_S(INFO) << "Cmd: `" << cmd << "'";

    stream = popen(cmd, "r");
    if (stream) {
        while (fgets(buffer, max_buffer, stream) != nullptr) {
            data.append(buffer);
        }
        if (ferror(stream)) LOG_S(ERROR) << "error getting stream for command: '" << cmd << "'";
        rtval = pclose(stream);
        LOG_S(INFO) << "Command return val: " << rtval;
    }
    std::unique_ptr<result_t> output = std::make_unique<result_t>();
    output->status = rtval;
    output->stdout = data;
    return output;
}

// One style of execution (not used currently)
int
exec(const char* file, const char* const argv[])
{
    std::size_t argc = 0;
    std::size_t len = 0;

    /* measure the inputs */
    for (auto* p = argv; *p; ++p) {
        LOG_S(INFO) << "Arg " << argc << ": " << argv[argc];
        ++argc;
        len += strlen(*p) + 1;
    }
    /* allocate copies */
    auto const arg_string = std::make_unique<char[]>(len);
    auto const args = std::make_unique<char*[]>(argc + 1);
    /* copy the inputs */
    len = 0; // re-use for position in arg_string
    for (auto i = 0u; i < argc; ++i) {
        len += strlen(args[i] = strcpy(&arg_string[len], argv[i])) +
               1; /* advance to one AFTER the nul */
    }
    args[argc] = nullptr;
    return execvp(file, args.get());
}

// Split string by delimiter
//
// @param str String to split
// @param delim Character to use as delimter
std::vector<std::string>
split(const std::string& str, char delim = ' ')
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
//
// @param str String view to split
// @param delims Delimiter(s)
std::vector<std::string_view>
split(const std::string_view str, std::string_view delims = " ")
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
const std::string
read_first_line(const char* filename)
{
    std::ifstream file;
    std::string line;
    file.open(filename);

    if (!file) {
        std::cerr << Ansi::setFg(Color::brred) << "Error opening file: " << Ansi::reset()
                  << filename << std::endl;
        LOG_S(ERROR) << "Could not open file: " << filename;
    } else {
        std::getline(file, line);
    }
    return line;
}

// Execute process and return result object
// Modified from original: github.com/ericcurtin/execxx
//
// @param args Arguments for command
// @param inc_stderr Capture stderr output
std::unique_ptr<result_t>
ex(const std::vector<std::string>& args, const bool inc_stderr)
{
    LOG_S(INFO) << "ex() cmd: " << args;
    // stdout
    int stdout_fds[2];
    pipe(stdout_fds);
    // stderr
    int stderr_fds[2];
    if (!inc_stderr) {
        pipe(stderr_fds);
    }

    const pid_t pid = fork();

    if (!pid) {
        close(stdout_fds[0]);
        dup2(stdout_fds[1], 1);
        if (inc_stderr) {
            dup2(stdout_fds[1], 2);
        }

        close(stdout_fds[1]);

        if (!inc_stderr) {
            close(stderr_fds[0]);
            dup2(stderr_fds[1], 2);
            close(stderr_fds[1]);
        }
        std::vector<char*> vc(args.size() + 1, nullptr);
        for (size_t i = 0; i < args.size(); ++i) {
            vc[i] = const_cast<char*>(args[i].c_str());
        }
        execvp(vc[0], &vc[0]);
    }

    close(stdout_fds[1]);

    std::string out;
    const int buf_size = 4096;
    char buffer[buf_size];
    do {
        const ssize_t r = read(stdout_fds[0], buffer, buf_size);
        if (r > 0) {
            out.append(buffer, r);
        }
    } while (errno == EAGAIN || errno == EINTR);

    close(stdout_fds[0]);

    if (!inc_stderr) {
        close(stderr_fds[1]);
        do {
            read(stderr_fds[0], buffer, buf_size);
        } while (errno == EAGAIN || errno == EINTR);
        close(stderr_fds[0]);
    }

    int r, status;
    do {
        r = waitpid(pid, &status, 0);
    } while (r == -1 && errno == EINTR);
    std::unique_ptr<result_t> cmd_result = std::make_unique<result_t>();
    cmd_result->status = status;
    cmd_result->stdout = out;
    return cmd_result;
}
