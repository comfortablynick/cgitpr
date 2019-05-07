#include "../config.h"
// #include "anyoption.h"
#include "common.h"
#include "loguru.hpp"
#include "repo.h"
#include "tclap/CmdLine.h"
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

#ifndef LOG_IS_ON
#define LOG_IS_ON(verbosity) ((loguru::Verbosity_##verbosity) <= loguru::current_verbosity_cutoff())
#endif

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
    LOG_S(1) << "Git status lines: " << lines;
    auto words = split(lines[0], " ");

    for (size_t i = 0; i < words.size(); i++) {
        LOG_S(2) << "Word: '" << words[i] << "'";
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
    LOG_S(INFO) << "HEAD: " << read_first_line(".git/HEAD");
    LOG_S(INFO) << "Ahead: " << ahead << "; Behind: " << behind;

    std::vector<std::string> dirty_cmd(5);
    dirty_cmd = {"git", "diff", "--no-ext-diff", "--quiet", " --exit-code"};
    std::unique_ptr<result_t> dirty_result = ex(dirty_cmd);

    LOG_S(INFO) << "`git diff --exit-code` command result: " << dirty_result->status;
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

    for (size_t i = 0; i < fmt.length(); ++i) {
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
int
parseFmtStr(std::shared_ptr<Options> opts)
{
    std::string& fmt = opts->format;

    for (size_t i = 0; i < fmt.length(); ++i) {
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

// void
// parseArgs(int argc, char** argv, std::shared_ptr<Options> opts)
// {
//     AnyOption* opt = new AnyOption;
//
//     // options
//     opt->setVerbose();         // print warnings about unknown options
//     opt->autoUsagePrint(true); // print usage for bad options
//
//     // set usage/help
//     opt->addUsage("usage: ");
//
//     // set options
//     opt->setCommandFlag("help", 'h');
//     opt->setCommandFlag("version", 'V');
//
//     // process options
//     opt->processCommandArgs(argc, argv);
//     if (opt->getFlag("help") || opt->getFlag('h')) opt->printUsage();
//     delete opt;
// }

class CustomHelpOutput : public TCLAP::StdOutput
{
  public:
    // TODO: break this into components for easy printing
    virtual void usage(TCLAP::CmdLineInterface& c)
    {
        // std::cerr << "USAGE: " << PACKAGE_NAME << " [FLAGS] [OPTIONS]\n"
        //           << PACKAGE_DESCRIPTION << "\n\n";
        // std::list<TCLAP::Arg*> args = c.getArgList();
        // for (TCLAP::ArgListIterator it = args.begin(); it != args.end(); it++) {
        //     spacePrint(std::cerr, (*it)->longID(), 75, 3, 3);
        //     spacePrint(std::cerr, (*it)->getDescription(), 75, 7, 0);
        // }
        printShortHelp();
        printHelpEpilog();
    }
};

// Parse argv with TCLAP and update Options.
//
// @param argc Argument count
// @param argv Command line arguments
// @param opts Options object
void
parseArgs(int argc, char** argv, std::shared_ptr<Options> opts)
{
    using namespace TCLAP;
    try {
        CmdLine cmd(PACKAGE_DESCRIPTION, ' ', PACKAGE_VERSION);
        CustomHelpOutput cho;
        cmd.setOutput(&cho);

        // add args
        SwitchArg quiet("q", "quiet", "Silences console debug output.", cmd, false);
        SwitchArg simple("s", "simple", "Emulates bash git prompt.", cmd, false);
        ValueArg<std::string> verbose("v", "verbose", "Increases console debug output.", false,
                                      "-2", "string", cmd);

        // parse args
        cmd.parse(argc, argv);
        opts->simple_mode = simple.getValue();
        opts->debug_quiet = simple.getValue();

    } catch (ArgException& e) {
        LOG_F(ERROR, "%s for arg %s", e.error(), e.argId());
    }
}

// Parse argv with getopt_long and update Options.
//
// @param argc Argument count
// @param argv Command line arguments
// @param opts Options object
void
processArgs(int argc, char** argv, std::shared_ptr<Options> opts)
{
    const char* const short_opts = "d:f:v:qhVistn";
    const option long_opts[] = {{"dir", required_argument, nullptr, 'd'},
                                {"verbose", required_argument, nullptr, 'v'},
                                {"format", required_argument, nullptr, 'f'},
                                {"help", no_argument, nullptr, 1},
                                {"version", no_argument, nullptr, 'V'},
                                {"quiet", no_argument, nullptr, 'q'},
                                {"indicators-only", no_argument, nullptr, 'i'},
                                {"no-color", no_argument, nullptr, 'n'},
                                {"simple", no_argument, nullptr, 's'},
                                {nullptr, no_argument, nullptr, 0}};

    for (;;) {
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
            break;
        case 'V':
            std::cerr << Ansi::setBg(Color::green) << argv[0] << Ansi::reset() << " "
                      << PACKAGE_VERSION << '\n';
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
            loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
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
        case ':':
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

    // Manipulate arguments if needed
    std::vector<std::string> arguments(argv, argv + argc);
    // if (argc == 1) {
    //     arguments.push_back("-v");
    //     arguments.push_back("INFO");
    // }

    std::vector<char*> args;
    for (auto& str : arguments) {
        args.push_back(&str.front());
    }
    int arg_ct = args.size();
    // processArgs(arg_ct, args.data(), opts);
    parseArgs(arg_ct, args.data(), opts);

    std::shared_ptr<termsize> tsize = getTermSize();

    // init loguru
    loguru::g_flush_interval_ms = 100;
    if (tsize->cols < 200) {
        loguru::g_preamble_thread = false;
        loguru::g_preamble_date = false;
        loguru::g_preamble_time = false;
        if (tsize->cols < 100) {
            loguru::g_preamble_file = false;
        }
    }
    if (opts->verbosity <= 0 || opts->debug_quiet) loguru::g_stderr_verbosity = -9;
    loguru::init(arg_ct, args.data(), "-v");

    LOG_F(1, "Term size: %dx%d", tsize->cols, tsize->lines);

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
        LOG_S(INFO) << "Repo is dirty; running git diff for numstat";
        std::unique_ptr<result_t> git_diff = run("git diff --shortstat");
        std::vector<std::string> diff_lines = split(git_diff->stdout, '\n');
        for (size_t i = 0; i < diff_lines.size(); i++) {
            ri->parseGitDiff(diff_lines[i]);
        }
    } else {
        LOG_S(INFO) << "Repo is not dirty; git diff not called";
    }

    LOG_S(INFO) << ri;
    LOG_S(INFO) << opts;

    std::cout << printOutput(opts, ri) << std::endl;
}
