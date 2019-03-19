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

/**
 * Globals and singleton classes
 */
static const char* version = "0.0.1";
Options* Options::instance = nullptr;
Repo* Repo::instance = nullptr;

/**
 * Parse tokenized format string (-f) and set Options
 * based on user input.
 */
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

/**
 * Output program help text to stderr.
 */
void printShortHelp(void)
{
    std::cerr << rang::fgB::green << "cgitpr " << rang::style::reset << version
              << "\ngit repo status for your prompt, written in C++.\n"
              << "Nick Murphy <comfortablynick@gmail.com>\n"
              << rang::fgB::yellow << "\nUSAGE:\n"
              << rang::fgB::green << "  cgitpr [FLAGS] [OPTIONS]\n"
              << rang::fgB::yellow << "\nFLAGS:\n"
              << rang::style::reset << "  -h, --help     Show this help message and exit\n"
              << "  -V, --version  Print version and exit\n"
              << "  -d, --debug    Print debug messages to console\n"
              << rang::fgB::yellow << "\nOPTIONS:\n"
              << rang::style::reset
              << "  -v, --verbose <n>   Log verbosity [1-9]; omitting <n> is the same as --v=9\n"
                 "  -f, --format [fmt]  Tokenized format string for git status\n"
                 "  --dir [dir]         Git directory (default is current directory)\n"
              << std::endl;
}

/**
 * Print extra help details to stderr if --help is called.
 */
void printHelpEpilog(void)
{
    std::cerr << "Format string may contain:\n"
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

/**
 * Parse argv with getopt_long and update Options.
 */
void processArgs(int argc, char** argv, Options* opts)
{
    const char* const short_opts = ":dv::f:hV";
    const option long_opts[] = {
        {"dir", required_argument, nullptr, 2}, {"verbose", optional_argument, nullptr, 'v'},
        {"debug", no_argument, nullptr, 'd'},   {"format", required_argument, nullptr, 'f'},
        {"help", no_argument, nullptr, 1},      {"version", no_argument, nullptr, 'V'},
        {nullptr, no_argument, nullptr, 0}};

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt) break;

        switch (opt) {
        // Short and short/long opts
        case 'h':
            printShortHelp();
            exit(1);
            break;
        case 'd':
            opts->debug_print = true;
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
        // Long-only opts (by number instead of char flag)
        case 1:
            printShortHelp();
            printHelpEpilog();
            exit(1);
            break;
        case 2:
            opts->dir = std::string(optarg);
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

/**
 * CLI entry point; direct to other funcs based on args.
 */
int main(int argc, char* argv[])
{
    // Logger setup
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Logger* console = el::Loggers::getLogger("default");
    console->enabled(el::Level::Warning); // This doesn't seem to work as expected

    Options* opts = opts->getInstance(); // static singleton

    // TODO: remove after testing
    std::vector<std::string> arguments(argv, argv + argc);
    if (arguments.size() == 1) arguments.push_back("-v");

    char** args = new char*[arguments.size() + 1];
    std::transform(arguments.begin(), arguments.end(), &args[0],
                   [](std::string& s) -> char* { return s.data(); });

    processArgs(arguments.size(), args, opts);

    if (opts->debug_print) {
        console->enabled(el::Level::Trace);
        LOG(INFO) << "Trace log level enabled!";
    }

    // log raw cli arguments
    if (VLOG_IS_ON(2)) {
        VLOG(2) << "Command Line Arguments:";
        VLOG(2) << "Arg count (argc): " << argc;
        for (size_t i = 0; i < arguments.size(); i++) {
            VLOG(2) << "Arg " << i << ": " << args[i];
        }
    }

    // TODO: check if in git repo
    CHECK(opts->format != "");

    Repo* ri = ri->getInstance();
    RepoArea* unstaged = new RepoArea();
    RepoArea* staged = new RepoArea();

    ri->Unstaged = *unstaged;
    ri->Staged = *staged;

    const std::string gitStatus = run("git status --porcelain=2 --branch");
    std::vector<std::string> statusLines = split(gitStatus, '\n');
    for (size_t i = 0; i < statusLines.size(); i++) {
        ri->parseGitStatus(statusLines[i]);
    }

    if (parseFmtStr(opts) == 1) return 1;

    // call git diff only if there are unstaged changes
    if (opts->show_diff == 1 && ri->Unstaged.hasChanged()) {
        VLOG(2) << "Repo is dirty; running git diff for numstat";
        const std::string gitDiff = run("git diff --shortstat");
        std::vector<std::string> diffLines = split(gitDiff, '\n');
        for (size_t i = 0; i < diffLines.size(); i++) {
            ri->parseGitDiff(diffLines[i]);
        }
    } else {
        VLOG(2) << "Repo is not dirty; git diff not called";
    }

    VLOG(1) << ri;
    VLOG(1) << opts;

    std::cout << opts->format << std::endl;

    delete unstaged;
    delete staged;
    return 0;
}
