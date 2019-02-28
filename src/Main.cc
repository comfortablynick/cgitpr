#include "Options.hpp"
#include "Repo.hpp"
#include "Utils.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "rang.hpp"
#include <getopt.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

// init singleton class pointers
Options* Options::instance = nullptr;
Repo* Repo::instance = nullptr;

// command-line flags
DEFINE_string(format, "%g %b@%c %a %m %u %s",
              "tokenized string for formatting output");
DEFINE_bool(debug, false, "print debug logs");
DEFINE_bool(h, false, "print short help for cgitpr only");

int parseFmtStr(Options* opts) {
    const std::string& fmt = FLAGS_format;

    for (std::string::size_type i = 0; i < FLAGS_format.length(); i++) {
        if (FLAGS_format[i] == '%') {
            i++;
            switch (FLAGS_format[i]) {
            case '\0': // end of string
                break;
            case 'a':
                opts->show_ahead_behind = true;
                break;
            case 'b':
                opts->show_branch = true;
                break;
            case 'c':
                opts->show_commit = true;
                break;
            case 'd':
                opts->show_diff = true;
                break;
            case 'g':
                opts->show_branch_glyph = true;
                break;
            case 'm':
                opts->show_unstaged_modified = true;
                break;
            case 'n':
                opts->show_vcs = true;
                break;
            case 'r':
                opts->show_remote = true;
                break;
            case 's':
                opts->show_staged_modified = true;
                break;
            case 't':
                opts->show_stashed = true;
                break;
            case 'u':
                opts->show_untracked = true;
                break;
            case '%':
                break;
            default:
                std::cerr << "error: token '" << FLAGS_format[i]
                          << "' not recognized\n";
                // exit(1);
                return 1;
            }
        }
    }
    return 0;
}

void printShortHelp(void) {
    std::cerr
        << "cgitpr: git repo status for your prompt, written in C++.\n"
        << rang::fgB::yellow << "\nUSAGE:\n"
        << rang::fgB::green << "  cgitpr [FLAGS] [OPTIONS]\n"
        << rang::fgB::yellow << "\nFLAGS:\n"
        << rang::style::reset
        << "  -h           Show this help message and exit\n"
           "  --help       Show all options from all packages\n"
        << rang::fgB::yellow << "\nOPTIONS:\n"
        << rang::style::reset
        << "  -v <n>              Log verbosity\n"
           "  -d, --dir <dir>     Git directory (default: \".\")\n"
           "  -f, --format <fmt>  Tokenized format string for git status\n"
        << rang::fgB::yellow << "\nFORMAT STRING:\n"
        << rang::style::reset
        << "Format string may contain:\n"
           "  %g  branch glyph ()\n"
           "  %n  VC name\n"
           "  %b  branch\n"
           "  %r  remote\n"
           "  %a  commits ahead/behind remote\n"
           "  %c  current commit hash\n"
           "  %m  unstaged changes (modified/added/removed)\n"
           "  %s  staged changes (modified/added/removed)\n"
           "  %u  untracked files\n"
           "  %d  diff lines, ex: \"+20/-10\"\n"
           "  %t  stashed files indicator\n"
        << rang::style::bold
        << "\nNote: all arguments may be prefixed with either `-` or `--`"
        << rang::style::reset << std::endl;
}

int main(int argc, char* argv[]) {
    // logger init
    google::InitGoogleLogging(argv[0]);
    google::SetStderrLogging(google::INFO);

    // arg parsing
    gflags::SetUsageMessage("git repo status for shell prompt, written in C++");
    gflags::SetVersionString("0.0.1");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_h) {
        printShortHelp();
        return 1;
    }

    Options* opts = opts->getInstance(); // static singleton
    Repo* ri = ri->getInstance();        // static singleton
    RepoArea* unstaged = new RepoArea();
    RepoArea* staged = new RepoArea();

    opts->format = FLAGS_format;
    opts->debug_print = true;
    ri->Unstaged = *unstaged;
    ri->Staged = *staged;

    const std::string gitStatus = run("git status --porcelain=2 --branch");
    std::vector<std::string> statusLines = split(gitStatus, '\n');
    for (size_t i = 0; i < statusLines.size(); i++) {
        ri->parseGitStatus(statusLines[i]);
    }

    VLOG(1) << ri;

    if (parseFmtStr(opts) == 1) {
        return 1;
    }

    VLOG(1) << opts;

    VLOG(2) << "All command line flags:\n"
            << gflags::CommandlineFlagsIntoString() << std::endl;
    std::cout << "Format string: " << FLAGS_format << std::endl;

    delete unstaged;
    delete staged;

    gflags::ShutDownCommandLineFlags();
    return 0;
}
