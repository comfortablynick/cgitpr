#include "Options.h"
#include "Repo.h"
#include "Utils.h"
#include "easylogging++.h"
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

INITIALIZE_EASYLOGGINGPP
#define ELPP_STL_LOGGING

// Globals and singleton classes
static const char* VERSION = "0.0.1";
static const bool DEBUG_MODE = true;
Options* Options::instance = nullptr;
Repo* Repo::instance = nullptr;

// Simple prompt emulating default bash promt that
// ships with git.
std::string simplePrompt()
{
    std::ostringstream ss;
    std::string_view branch;
    bool dirty, ahead, behind;
    const std::string git_status = run("git status --porcelain --branch --untracked-files=no");
    auto lines = split(git_status, "\n");
    VLOG(2) << "Git status lines: " << lines;
    auto words = split(lines[0], " ");
    for (size_t i = 0; i < words.size(); i++) {
        VLOG(3) << "Word: '" << words[i] << "'";
        if (words[i] == "##") {
            i++;
            branch = split(words[i], "...")[0];
        }
        if (words[i] == "[behind") {
            behind = true;
            VLOG(3) << "Local branch is behind remote";
        }
        if (words[i] == "[ahead") {
            ahead = true;
            VLOG(3) << "Local branch is ahead of remote";
        }
    }
    VLOG(3) << "HEAD: " << read_first_line(".git/HEAD");
    if (lines.size() > 1) dirty = true;
    ss << Ansi::setFg(Color::cyan) << "(" << branch << ")";
    if (dirty) ss << Ansi::setFg(Color::red) << "*";
    ss << Ansi::reset();
    return ss.str();
}

std::string printOutput(Options* opts, Repo* ri)
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
                ss << ri->Unstaged.formatModified(ind);
                break;
            case 'n':
                ss << "git";
                break;
            case 'r':
                ss << ri->remote;
                break;
            case 's':
                ss << ri->Staged.formatModified(ind);
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
int parseFmtStr(Options* opts)
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
void printShortHelp(void)
{
    std::cerr << Ansi::setFg(Color::green) << "cgitpr " << Ansi::reset() << VERSION
              << "\ngit repo status for your prompt, written in C++.\n"
              << "Nick Murphy <comfortablynick@gmail.com>\n"
              << Ansi::setFg(Color::bryellow) << "\nUSAGE:\n"
              << Ansi::setFg(Color::green) << "  cgitpr [FLAGS] [OPTIONS]\n"
              << Ansi::setFg(Color::bryellow) << "\nFLAGS:\n"
              << Ansi::reset()
              << "  -h, --help             Show short or long help message and exit\n"
              << "  -V, --version          Print version and exit\n"
              << "  -d, --debug            Print debug messages to console\n"
              << "  -s, --simple           Simple output, similar to default bash git prompt\n"
              << "  -n, --no-color         Disable color in output\n"
              << "  -q, --quiet            Quiet debug output (overrides -v/-d)\n"
              << "  -i, --indicators-only  Show symbols only in output (no counts)\n"
              << Ansi::setFg(Color::bryellow) << "\nOPTIONS:\n"
              << Ansi::reset()
              << "  -v, --verbose <n>      Log verbosity [1-9]; (default: 9)\n"
                 "  -f, --format FORMAT    Tokenized format string for git status\n"
                 "  --dir DIRECTORY        Git directory, if different from current"
              << std::endl;
}

/**
 * Print extra help details to stderr if --help is called.
 */
void printHelpEpilog(void)
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
void processArgs(int argc, char** argv, Options* opts)
{
    const char* const short_opts = ":dv::f:qhVisn";
    const option long_opts[] = {
        {"dir", required_argument, nullptr, 2},  {"verbose", optional_argument, nullptr, 'v'},
        {"debug", no_argument, nullptr, 'd'},    {"format", required_argument, nullptr, 'f'},
        {"help", no_argument, nullptr, 1},       {"version", no_argument, nullptr, 'V'},
        {"quiet", no_argument, nullptr, 'q'},    {"indicators-only", no_argument, nullptr, 'i'},
        {"no-color", no_argument, nullptr, 'n'}, {"simple", no_argument, nullptr, 's'},
        {nullptr, no_argument, nullptr, 0}};

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
            if (optarg != NULL) {
                el::Loggers::setVerboseLevel(std::atoi(optarg));
            } else {
                el::Loggers::setVerboseLevel(9);
            }
            LOG(INFO) << "processArgs: Verbosity set to: " << el::Loggers::verboseLevel();
            break;
        case 'V':
            std::cerr << Ansi::setBg(Color::green) << argv[0] << Ansi::reset() << " " << VERSION
                      << "\n";
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
            opts->format = std::string(optarg);
            break;
        case 'q':
            opts->debug_quiet = true;
            opts->debug_print = false;
            el::Loggers::setVerboseLevel(0);
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

// CLI entry point; direct to other funcs based on args.
int main(int argc, char* argv[])
{
    Options* opts = opts->getInstance(); // singleton options object

    // Logger init
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Logger* console = el::Loggers::getLogger("default");
    console->enabled(el::Level::Warning); // This doesn't seem to work as expected

    if (opts->debug_print) {
        console->enabled(el::Level::Trace);
        LOG(INFO) << "Trace log level enabled!";
    }

    // TODO: remove test argv+argc after testing
    std::vector<std::string> arguments(argv, argv + argc);
    // if (arguments.size() == 1) arguments.push_back("-v");

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
    delete[] args;

    // TODO: check if in git repo
    CHECK(opts->format != "");

    if (opts->simple_mode) {
        std::cout << simplePrompt() << std::endl;
        return 0;
    }

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

    std::cout << printOutput(opts, ri) << std::endl;

    delete unstaged;
    delete staged;
    return 0;
}
