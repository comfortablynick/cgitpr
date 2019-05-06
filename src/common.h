#ifndef COMMON_H
#define COMMON_H

#include <fstream>
#include <iomanip>
#include <memory>
#include <stdio.h>
#include <string>
#include <string_view>
#include <vector>

template <typename... Args>
std::string
fmt(const std::string& format, Args... args)
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
std::ostream&
operator<<(std::ostream& out, const std::vector<T>& vec)
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
std::string
prettify(const std::vector<T>& vec)
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


std::string
prettify(const char*[]);
std::unique_ptr<result_t>
run(const char*);
int
exec(const char*, const char* const[]);
std::vector<std::string>
split(const std::string&, char);
std::vector<std::string_view>
split(const std::string_view, std::string_view);
std::unique_ptr<result_t>
ex(const std::vector<std::string>&, const bool = false);

// Value on the Ansi 256 color spectrum
enum class Color : unsigned int
{
    // std colors
    black = 0,
    blue = 12,
    green = 2,
    cyan = 37,
    red = 124,
    yellow = 142,
    gray = 245,

    // bright colors
    brcyan = 51,
    brred = 196,
    bryellow = 226,
};

namespace Ansi {
    std::string reset();
    std::string setFg(Color);
    std::string setBg(Color);
} // namespace Ansi

const std::string
read_first_line(const char*);

class Options
{
  public:
    bool debug_print, debug_quiet, indicators_only, show_ahead_behind, show_branch,
        show_branch_glyph, show_commit, show_diff, show_remote, show_stashed, show_staged_modified,
        show_unstaged_modified, show_untracked, show_vcs, no_color, simple_mode;
    std::string format, dir;
    int verbosity;
    Options()
        : debug_print(false), debug_quiet(false), indicators_only(false), show_ahead_behind(false),
          show_branch(false), show_branch_glyph(false), show_commit(false), show_diff(false),
          show_remote(false), show_stashed(false), show_staged_modified(false),
          show_unstaged_modified(false), show_untracked(false), show_vcs(false), no_color(false),
          simple_mode(false), format("%g %b@%c %a %m %d %s %u %t"), dir("."), verbosity(0)
    {}

    friend std::ostream& operator<<(std::ostream& out, Options* obj)
    {
        out << "Options:" << '\n';
        out << "  Show ahead/behind: " << obj->show_ahead_behind << '\n';
        out << "  Show branch: " << obj->show_branch << '\n';
        out << "  Show branch glyph: " << obj->show_branch_glyph << '\n';
        out << "  Show commit: " << obj->show_commit << '\n';
        out << "  Show diff: " << obj->show_diff << '\n';
        out << "  Show remote: " << obj->show_remote << '\n';
        out << "  Show stashed: " << obj->show_stashed << '\n';
        out << "  Show staged modified: " << obj->show_staged_modified << '\n';
        out << "  Show unstaged modified: " << obj->show_unstaged_modified << '\n';
        out << "  Show untracked: " << obj->show_untracked << '\n';
        out << "  Show vcs: " << obj->show_vcs << '\n';
        out << "  Debug print: " << obj->debug_print << '\n';
        out << "  Debug quiet: " << obj->debug_quiet << '\n';
        out << "  No color: " << obj->no_color << '\n';
        out << "  Simple mode: " << obj->simple_mode << '\n';
        out << "  Indicators only: " << obj->indicators_only << '\n';
        out << "  Format: " << obj->format << '\n';
        out << "  Dir: " << obj->dir << '\n';
        out << "  Verbosity: " << obj->verbosity << '\n';
        return out;
    }
};

#endif // COMMON_H
