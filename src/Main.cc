#include "../config.h"
#include "Options.h"
#include "Repo.h"
#include "Utils.h"
#include "loguru.hpp"
#include <algorithm>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <ext/alloc_traits.h>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <string_view>
#include <vector>

// Simple prompt emulating default bash prompt that
// ships with git.
const std::string
simplePrompt()
{
    std::ostringstream ss;
    std::string_view branch;
    bool dirty, ahead, behind = false;
    std::vector<std::string> status_cmd(5);
    status_cmd = {"git", "status", "--porcelain", "--branch", "--untracked-files=no"};
    std::unique_ptr<result_t> git_status = ex(status_cmd, true);
    if (git_status->status != 0) {
        std::cerr << Ansi::setFg(Color::brred) << git_status->stdout << Ansi::reset();
        exit(EXIT_FAILURE);
    }
    auto lines = split(git_status->stdout, "\n");
    VLOG(INFO) << "Git status lines: " << lines;
    auto words = split(lines[0], " ");

    for (size_t i = 0; i < words.size(); i++) {
        VLOG(INFO) << "Word: '" << words[i] << "'";
        if (words[i] == "##") {
            i++;
            branch = split(words[i], "...")[0];
        }
        if (words[i] == "[behind") {
            behind = true;
        }
        if (words[i] == "[ahead") {
            ahead = true;
        }
    }
    VLOG(INFO) << "HEAD: " << read_first_line(".git/HEAD");
    VLOG(INFO) << "Ahead: " << ahead << "; Behind: " << behind;

    std::vector<std::string> dirty_cmd(5);
    dirty_cmd = {"git", "diff", "--no-ext-diff", "--quiet", " --exit-code"};
    std::unique_ptr<result_t> dirty_result = ex(dirty_cmd);

    VLOG(INFO) << "`git diff --exit-code` command result: " << dirty_result->status;
    dirty = dirty_result->status == 0 ? false : true;

    ss << Ansi::setFg(Color::cyan) << "(" << branch << ")";
    if (dirty) {
        ss << Ansi::setFg(Color::red) << "*";
    }
    ss << Ansi::reset();
    return ss.str();
}

const std::string
printOutput(std::shared_ptr<Options> opts, std::shared_ptr<Repo> ri)
{
    std::stringstream ss;
    std::string& fmt = opts->format;
    bool ind = opts->indicators_only;

    for (size_t i = 0; i < fmt.length(); i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
            case '\0':
                break;
            case 'a':
                ss << ri->formatAheadBehind(ind);
                break;
            case 'b':
                ss << ri->formatBranch();
                break;
            case 'c':
                ss << ri->formatCommit(7);
                break;
            case 'd':
                ss << ri->formatDiff();
                break;
            case 'g':
                ss << ri->BRANCH_GLYPH;
                break;
            case 'm':
                ss << ri->Unstaged->formatModified(ind);
                break;
            case 'n':
                ss << "git";
                break;
            case 'r':
                ss << ri->remote;
                break;
            case 's':
                ss << ri->Staged->formatModified(ind);
                break;
            case 't':
                ss << ri->formatStashed(ind);
                break;
            case 'u':
                ss << ri->formatUntracked(ind);
                break;
            case '%':
                ss << '%';
                break;
            default:
                std::cerr << "error: token '" << fmt[i] << "' not recognized\n";
                exit(1);
            }
        } else {
            ss << fmt[i];
        }
    }
    return ss.str();
}

// Parse tokenized format string (-f) and set Options
// based on user input.
//
// @param opts Options object
int
parseFmtStr(std::shared_ptr<Options> opts)
{
    std::string& fmt = opts->format;

    for (size_t i = 0; i < fmt.length(); i++) {
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
static void
printShortHelp()
{
    std::cerr << Ansi::setFg(Color::green) << PACKAGE_STRING << Ansi::reset() << '\n'
              << PACKAGE_DESCRIPTION << '\n'
              << PACKAGE_URL << '\n'
              << Ansi::setFg(Color::bryellow) << "\nUSAGE:\n"
              << Ansi::setFg(Color::green) << "  cgitpr [FLAGS] [OPTIONS]\n"
              << Ansi::setFg(Color::bryellow) << "\nFLAGS:\n"
              << Ansi::reset()
              << "  -h, --help             Show short or long help message and exit\n"
                 "  -V, --version          Print version and exit\n"
                 "  -v, --verbose          Log verbosity; ex: -v, -vv, -vvv\n"
                 "  -s, --simple           Simple output, similar to default bash git prompt\n"
                 "  -n, --no-color         Disable color in output\n"
                 "  -q, --quiet            Quiet debug output (overrides -v/-d)\n"
                 "  -i, --indicators-only  Show symbols only in output (no counts)\n"
              << Ansi::setFg(Color::bryellow) << "\nOPTIONS:\n"
              << Ansi::reset()
              << "  -f, --format FORMAT    Tokenized format string for git status\n"
                 "  -d, --dir DIRECTORY    Git directory, if different from current"
              << std::endl;
}

/**
 * Print extra help details to stderr if --help is called.
 */
static void
printHelpEpilog()
{
    std::cerr << Ansi::setFg(Color::bryellow) << "\nFormat string may contain:\n"
              << Ansi::reset()
              << "  %g  branch glyph ()\n"
                 "  %n  VC name\n"
                 "  %b  branch\n"
                 "  %r  remote\n"
                 "  %a  commits ahead/behind remote\n"
                 "  %c  current commit hash\n"
                 "  %m  unstaged changes (modified/added/removed)\n"
                 "  %s  staged changes (modified/added/removed)\n"
                 "  %u  untracked files\n"
                 "  %d  diff lines, ex: \"+20/-10\"\n"
                 "  %t  stashed files indicator"
              << std::endl;
}

// Parse argv with getopt_long and update Options.
//
// @param argc Argument count
// @param argv Command line arguments
// @param opts Options object
void
processArgs(int argc, char** argv, std::shared_ptr<Options> opts)
{
    const char* const short_opts = ":d:f:qhVvisn";
    const option long_opts[] = {
        {"dir", required_argument, nullptr, 'd'},       {"verbose", no_argument, nullptr, 'v'},
        {"format", required_argument, nullptr, 'f'},    {"help", no_argument, nullptr, 1},
        {"version", no_argument, nullptr, 'V'},         {"quiet", no_argument, nullptr, 'q'},
        {"indicators-only", no_argument, nullptr, 'i'}, {"no-color", no_argument, nullptr, 'n'},
        {"simple", no_argument, nullptr, 's'},          {nullptr, no_argument, nullptr, 0}};

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if (opt == -1) break;
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
            ++opts->verbosity;
            break;
        case 'V':
            std::cerr << Ansi::setBg(Color::green) << argv[0] << Ansi::reset() << " "
                      << PACKAGE_VERSION << "\n";
            exit(1);
        case 'i':
            opts->indicators_only = true;
            break;
        case 'n':
            opts->no_color = true;
            setenv("TERM", "dumb", 1);
            break;
        case 's':
            opts->simple_mode = true;
            break;
        case 'f':
            opts->format = std::string(optarg);
            break;
        case 'q':
            opts->debug_quiet = true;
            opts->debug_print = false;
            // el::Loggers::setVerboseLevel(0);
            // loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
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
            VLOG(ERROR) << "Parsing error: unknown opt '" << (char)optopt << "'\n";
            printShortHelp();
            exit(1);
        case ':':
            VLOG(ERROR) << "Parsing error: missing arg for '" << (char)optopt << "'\n";
            printShortHelp();
            exit(1);
        default:
            printShortHelp();
            exit(1);
        }
    }
}

// CLI entry point; direct to other funcs based on args.
int
main(int argc, char* argv[])
{
    std::shared_ptr<Options> opts = std::make_shared<Options>();

#ifdef EASYLOGGINGPP_H
    // Logger init
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Logger* console = el::Loggers::getLogger("default");
    console->enabled(el::Level::Warning); // This doesn't seem to work as expected
#endif

    // TODO: remove test argv+argc after testing
    std::vector<std::string> arguments(argv, argv + argc);
    // if (arguments.size() == 1) arguments.push_back("-v");

    char** args = new char*[arguments.size() + 1];
    std::transform(arguments.begin(), arguments.end(), &args[0],
                   [](std::string& s) -> char* { return s.data(); });

    processArgs(arguments.size(), args, opts);

#ifdef LOGURU_HAS_DECLARED_FORMAT_HEADER
    // init loguru
    loguru::g_flush_interval_ms = 100;
    loguru::g_preamble_thread = false;
    // loguru::g_stderr_verbosity = loguru::Verbosity_INFO;
    loguru::init(argc, argv, nullptr);
#endif

    // log raw cli arguments
    if (VLOG_IS_ON(INFO)) {
        VLOG(INFO) << "Command Line Arguments:";
        VLOG(INFO) << "Arg count (argc): " << argc;
        for (size_t i = 0; i < arguments.size(); i++) {
            VLOG(INFO) << "Arg " << i << ": " << args[i];
        }
    }
    delete[] args;

    // TODO: check if in git repo
    CHECK_F(opts->format != "");

    if (opts->simple_mode) {
        std::cout << simplePrompt() << std::endl;
        return 0;
    }

    std::shared_ptr<Repo> ri = std::make_shared<Repo>();

    if (parseFmtStr(opts) == 1) return 1;
    std::vector<std::string> status_args(5);
    status_args = {"git", "status", "--porcelain=2", "--branch", "--untracked-files=no"};

    if (opts->show_untracked == 1) {
        status_args[4] = "--untracked-files=all";
        // normal: show untracked files and dirs
        // all:    show individual files within dirs
    }

    std::unique_ptr<result_t> git_status = ex(status_args);
    if (git_status->status != 0) {
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> status_lines = split(git_status->stdout, '\n');
    for (size_t i = 0; i < status_lines.size(); i++) {
        ri->parseGitStatus(status_lines[i]);
    }

    // call git diff only if there are unstaged changes
    if (opts->show_diff == 1 && ri->Unstaged->hasChanged()) {
        VLOG(INFO) << "Repo is dirty; running git diff for numstat";
        std::unique_ptr<result_t> git_diff = run("git diff --shortstat");
        std::vector<std::string> diff_lines = split(git_diff->stdout, '\n');
        for (size_t i = 0; i < diff_lines.size(); i++) {
            ri->parseGitDiff(diff_lines[i]);
        }
    } else {
        VLOG(INFO) << "Repo is not dirty; git diff not called";
    }

    VLOG(INFO) << ri;
    VLOG(INFO) << opts;

    std::cout << printOutput(opts, ri) << std::endl;
}
