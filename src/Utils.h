#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iomanip>
#include <memory>
#include <stdio.h>
#include <string>
#include <string_view>
#include <vector>

template <typename... Args>
std::string fmt(const std::string& format, Args... args)
{
    size_t size = 1 + std::snprintf(nullptr, 0, format.c_str(), args...);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size);
}

// Output a text representation of vector to stream.
// For pretty output, use prettify() to get string first.
// @param out Stream to print to
// @param vec Vector to print
template <class T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
    out << '[';
    for (size_t i = 0; i < vec.size(); i++) {
        if (i != 0) out << ", ";
        out << vec[i];
    }
    out << ']';
    return out;
}

// Pretty print representation of vector.
// For simple debug print, use << operator on vector directly.
// @param vec Vector of <T> type
template <class T>
std::string prettify(const std::vector<T>& vec)
{
    std::ostringstream out;
    out << "[\n";
    for (size_t i = 0; i < vec.size(); i++) {
        if (i != 0) out << ",\n";
        out << std::setw(3) << std::setfill(' ') << i << " | " << vec[i];
    }
    out << "\n]";
    return out.str();
}

// Holds the output of a system command
struct result_t
{
    int status;
    std::string stdout;
};

std::unique_ptr<result_t> run(const char*);
int exec(const char*, const char* const[]);
std::vector<std::string> split(const std::string&, char);
std::vector<std::string_view> split(const std::string_view, std::string_view);
std::unique_ptr<result_t> ex(const std::vector<std::string>&, const bool = false);

enum class Color : unsigned int
{
    black = 0,
    blue = 12,
    green = 2,
    cyan = 37,
    red = 124,
    yellow = 142,
    brcyan = 51,
    brred = 196,
    bryellow = 226,
};
namespace Ansi {
    std::string reset(void);
    std::string setFg(Color);
    std::string setBg(Color);
} // namespace Ansi

const std::string read_first_line(const char*);
#endif // UTILS_H
