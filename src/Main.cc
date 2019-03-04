#include "Options.hpp"
#include "Repo.hpp"
#include "Utils.hpp"
#include "easylogging++.h"
#include "rang.hpp"
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

INITIALIZE_EASYLOGGINGPP
#define ELPP_STL_LOGGING

// globals
static const std::string version = "0.0.1";

// init singleton class pointers
Options* Options::instance = nullptr;
Repo* Repo::instance = nullptr;

int parseFmtStr(Options* opts)
{
    std::string& fmt = opts->format;

    for (std::string::size_type i = 0; i < fmt.length(); i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
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
                std::cerr << "error: token '" << fmt[i] << "' not recognized\n";
                exit(1);
            }
        }
    }
    return 0;
}

void printShortHelp(void)
{
    std::cerr << rang::fgB::green << "cgitpr " << rang::style::reset << version
              << "\ngit repo status for your prompt, written in C++.\n"
              << "Nick Murphy <comfortablynick@gmail.com>\n"
              << rang::fgB::yellow << "\nUSAGE:\n"
              << rang::fgB::green << "  cgitpr [FLAGS] [OPTIONS]\n"
              << rang::fgB::yellow << "\nFLAGS:\n"
              << rang::style::reset
              << "  -h      Show this help message and exit\n"
                 "  --help  Show all options from all packages\n"
              << rang::fgB::yellow << "\nOPTIONS:\n"
              << rang::style::reset
              << "  -v, --verbose <n>   Log verbosity [1-9]; omitting <n> is the same as --v=9\n"
                 "  -d, --dir <dir>     Git directory (default: \".\")\n"
                 "  -f, --format <fmt>  Tokenized format string for git status\n"
              << "\nFormat string may contain:\n"
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
              << std::endl;
}

void processArgs(int argc, char** argv, Options* opts)
{
    const char* const short_opts = ":d:v::f:d:h:V";
    const option long_opts[] = {
        {"dir", required_argument, nullptr, 'd'},    {"verbose", optional_argument, nullptr, 'v'},
        {"format", required_argument, nullptr, 'f'}, {"help", no_argument, nullptr, 'h'},
        {"version", no_argument, nullptr, 'V'},      {nullptr, no_argument, nullptr, 0}};

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt) break;

        switch (opt) {
        case 'h':
            printShortHelp();
            exit(1);
            break;
        case 'd':
            opts->dir = optarg;
            break;
        case 'v':
            if (optarg != NULL) {
                el::Loggers::setVerboseLevel(std::atoi(optarg));
            } else {
                el::Loggers::setVerboseLevel(9);
            }
            LOG(INFO) << "processArgs: Verbosity set to: " << el::Loggers::verboseLevel();
            break;
        case 'V':
            std::cerr << rang::fgB::green << argv[0] << rang::style::reset << " " << version
                      << "\n";
            exit(1);
        case 'f':
            opts->format = std::string(optarg);
            break;
        case '?':
            // TODO: does not print if not a char; figure out how to parse any values
            LOG(ERROR) << "Parsing error: unknown opt '" << (char)optopt << "'\n";
            printShortHelp();
            exit(1);
        case ':':
            LOG(ERROR) << "Parsing error: missing arg for '" << (char)optopt << "'\n";
            printShortHelp();
            exit(1);
        default:
            printShortHelp();
            exit(1);
        }
    }
}

int main(int argc, char* argv[])
{
    /* LOGGER */
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Logger* console = el::Loggers::getLogger("default"); // needed for printf-style logging
    console->enabled(el::Level::Trace);
    // el::Loggers::setLoggingLevel(el::Level::Trace);
    Options* opts = opts->getInstance(); // static singleton
    processArgs(argc, argv, opts);

    // log raw cli arguments
    if (VLOG_IS_ON(2)) {
        for (size_t i = 0; i < argc; i++) {
            VLOG(2) << "Arg " << i << ". " << argv[i];
        }
    }

    CHECK(opts->format != "");

    Repo* ri = ri->getInstance(); // static singleton
    RepoArea* unstaged = new RepoArea();
    RepoArea* staged = new RepoArea();

    opts->debug_print = true;
    ri->Unstaged = *unstaged;
    ri->Staged = *staged;

    const std::string gitStatus = run("git status --porcelain=2 --branch");
    std::vector<std::string> statusLines = split(gitStatus, '\n');
    for (size_t i = 0; i < statusLines.size(); i++) {
        ri->parseGitStatus(statusLines[i]);
    }

    VLOG(1) << ri;

    if (parseFmtStr(opts) == 1) return 1;

    VLOG(1) << opts;

    if (opts->show_diff == 1) {
        const std::string gitDiff = run("git diff --numstat");
        std::vector<std::string> diffLines = split(gitDiff, '\n');
        for (size_t i = 0; i < diffLines.size(); i++) {
            // VLOG(1) << diffLines[i];
            ri->parseGitDiff(diffLines[i]);
        }
    }

    std::cout << opts->format << std::endl;

    delete unstaged;
    delete staged;

    return 0;
}
