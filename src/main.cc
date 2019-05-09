// #define FMT_HEADER_ONLY
// #include <fmt/format.h>
#include "../config.h"
#include "common.h"
#include "repo.h"
#include <cstdlib>
#include <iostream>
#include <loguru.hpp>
#include <memory>
#include <stdio.h>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

#ifndef LOG_IS_ON
#define LOG_IS_ON(verbosity) ((loguru::Verbosity_##verbosity) <= loguru::current_verbosity_cutoff())
#endif

// Simple prompt emulating default bash prompt that
// ships with git.
const std::string simplePrompt()
{
    std::ostringstream ss;
    std::string_view branch;
    std::vector<std::string> status_cmd(5);
    status_cmd = {"git", "status", "--porcelain", "--branch", "--untracked-files=no"};
    auto git_status = ex(status_cmd, true);
    if (git_status->status != 0) {
        std::cerr << Ansi::setFg(Color::brred) << git_status->stdout << Ansi::reset();
        exit(EXIT_FAILURE);
    }
    auto lines = split(git_status->stdout, "\n");
    LOG_S(1) << "Git status lines: " << lines;
    auto words = split(lines[0], " ");

    bool ahead = false;
    bool behind = false;
    for (auto i = 0; i < words.size(); ++i) {
        LOG_S(2) << "Word: '" << words[i] << "'";
        if (words[i] == "##") {
            ++i;
            branch = split(words[i], "...")[0];
        }
        if (words[i] == "[behind") {
            behind = true;
        }
        if (words[i] == "[ahead") {
            ahead = true;
        }
    }
    LOG_S(INFO) << "HEAD: " << read_first_line(".git/HEAD");
    LOG_S(INFO) << "Ahead: " << ahead << "; Behind: " << behind;

    // Getting inconsistent results with the below command
    // std::vector<std::string> dirty_cmd(5);
    // dirty_cmd = {"git", "diff", "--no-ext-diff", "--quiet", " --exit-code"};
    // auto dirty_result = ex(dirty_cmd);
    //
    // LOG_S(INFO) << "`git diff --exit-code` command result: " << dirty_result->status;
    // bool dirty = dirty_result->status == 0 ? false : true;

    ss << Ansi::setFg(Color::cyan) << "(" << branch << ")";
    if (lines.size() > 1) ss << Ansi::setFg(Color::red) << "*";
    ss << Ansi::reset();
    return ss.str();
}

const std::string printOutput(std::shared_ptr<Options> opts, std::shared_ptr<Repo> ri)
{
    std::stringstream ss;
    const std::string& fmt = opts->format;
    auto ind = opts->indicators_only;

    for (auto i = 0; i < fmt.length(); ++i) {
        if (fmt[i] == '%') {
            ++i;
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
int parseFmtStr(std::shared_ptr<Options> opts)
{
    const std::string& fmt = opts->format;

    for (auto i = 0; i < fmt.length(); ++i) {
        if (fmt[i] == '%') {
            ++i;
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
 * Output program help text to stream.
 */
static void printShortHelp(std::ostream& str, bool color = true)
{
    char* orig_term;
    if (!color) {
        orig_term = getenv("TERM");
        setenv("TERM", "dumb", 1);
    }
    str << Ansi::setFg(Color::green) << PACKAGE_STRING << Ansi::reset() << '\n'
        << PACKAGE_DESCRIPTION << '\n'
        << PACKAGE_URL << '\n'
        << Ansi::setFg(Color::bryellow) << "\nUSAGE:\n"
        << Ansi::setFg(Color::green) << "  cgitpr [FLAG]... [OPTION]...\n"
        << Ansi::setFg(Color::bryellow) << "\nFLAGS:\n"
        << Ansi::reset()
        << "  -h, --help             Show short or long help message and exit\n"
           "  -V, --version          Print version and exit\n"
           "  -q, --quiet            Silence debug console output (overrides -v)\n"
           "  -s, --simple           Simple output, similar to default bash git prompt\n"
           "  -n, --no-color         Disable color in output\n"
           "  -i, --indicators-only  Show symbols only in output (no counts)\n"
        << Ansi::setFg(Color::bryellow) << "\nOPTIONS:\n"
        << Ansi::reset()
        << "  -f, --format FORMAT    Tokenized format string for git status\n"
           "  -d, --dir DIRECTORY    Git directory, if different from current\n"
           "  -v LEVEL               Log verbosity: OFF, ERROR, WARNING, INFO, 0-9\n";
    if (!color) setenv("TERM", orig_term, 1);
}

/**
 * Print extra help details to stream
 */
static void printHelpEpilog(std::ostream& str, bool color = true)
{
    char* orig_term;
    if (!color) {
        orig_term = getenv("TERM");
        setenv("TERM", "dumb", 1);
    }
    str << Ansi::setFg(Color::bryellow) << "\nFormat string may contain:\n"
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
    if (!color) setenv("TERM", orig_term, 1);
}


// Parse argv with getopt and update Options.
//
// @param argc Argument count
// @param argv Command line arguments
// @param opts Options object
int parseArgs(const int& argc, char** argv, std::shared_ptr<Options> opts)
{
    int opt;
    while ((opt = getopt(argc, argv, "d:f:v:qhHVistn")) != -1) {
        switch (opt) {
        case 'h':
            printShortHelp(std::cerr, false);
            return EXIT_FAILURE;
            break;
        case 'H':
            printShortHelp(std::cerr, true);
            printHelpEpilog(std::cerr);
            return EXIT_FAILURE;
            break;
        case 'd':
            opts->dir = std::string(optarg);
            break;
        case 'v':
            opts->verbosity = std::string(optarg);
            break;
        case 'V':
            std::cerr << PACKAGE_STRING << std::endl;
            return EXIT_FAILURE;
            break;
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
            opts->verbosity = "OFF";
            loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
            break;
        case '?':
        case ':':
            return EXIT_FAILURE;
        default:
            printShortHelp(std::cerr);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

// CLI entry point; direct to other funcs based on args.
int main(int argc, char* argv[])
{
    auto opts = std::make_shared<Options>();
    std::vector<std::string> arguments(argv, argv + argc);
    if (argc == 1) { // Manipulate arguments if needed
        arguments.push_back("-v");
        arguments.push_back("INFO");
    }

    std::vector<char*> args; // convert vector back to char**
    for (auto& str : arguments) {
        args.push_back(&str.front());
    }
    int arg_ct = args.size();
    if (parseArgs(args.size(), args.data(), opts) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    // init loguru
    loguru::g_flush_interval_ms = 100;
    if (auto tsize = getTermSize(); tsize->cols < 200) {
        LOG_F(1, "Log output adjusted for term size: %dx%d", tsize->cols, tsize->lines);
        loguru::g_preamble_thread = false;
        loguru::g_preamble_date = false;
        loguru::g_preamble_time = false;
        if (tsize->cols < 100) {
            loguru::g_preamble_file = false;
        }
    }
    if (opts->verbosity == "-2" || opts->debug_quiet) loguru::g_stderr_verbosity = -9;
    loguru::init(arg_ct, args.data(), "-v");

    // TODO: check if in git repo
    CHECK_F(opts->format != "");

    if (opts->simple_mode) {
        std::cout << simplePrompt() << std::endl;
        return 0;
    }

    auto ri = std::make_shared<Repo>();

    if (parseFmtStr(opts) == 1) return 1;
    std::vector<std::string> status_args(5);
    status_args = {"git", "status", "--porcelain=2", "--branch", "--untracked-files=no"};

    if (opts->show_untracked == 1) {
        // normal: show untracked files and dirs
        // all:    show individual files within dirs
        // no:     skip showing untracked files (default; faster)
        status_args[4] = "--untracked-files=all";
    }

    const auto git_status = ex(status_args);
    if (git_status->status != 0) {
        exit(EXIT_FAILURE);
    }
    auto status_lines = split(git_status->stdout, '\n');
    for (const auto& line : status_lines) {
        ri->parseGitStatus(line);
    }

    if (opts->show_diff && ri->Unstaged->hasChanged()) {
        // call git diff only if there are unstaged changes
        LOG_F(INFO, "Repo is dirty; running git diff for numstat");
        const auto git_diff = run("git diff --shortstat");
        const auto diff_lines = split(git_diff->stdout, '\n');
        for (const auto& line : diff_lines) {
            ri->parseGitDiff(line);
        }
    } else {
        LOG_F(INFO, "Repo is not dirty; git diff not called");
    }

    LOG_S(INFO) << ri;
    LOG_S(INFO) << opts;

    std::cout << printOutput(opts, ri) << std::endl;
}
