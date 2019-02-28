#include "Options.hpp"
#include "Repo.hpp"
#include "Utils.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

void parseFmtStr(Options *opts) {
  const std::string &fmt = opts->format;

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
}

DEFINE_string(format, "%g %b@%c %a %m %u %s",
              "tokenized string for formatting output");
DEFINE_bool(debug, false, "print debug logs");

int main(int argc, char *argv[]) {
  // logger init
  google::InitGoogleLogging(argv[0]);
  google::SetStderrLogging(google::INFO);

  // arg parsing
  gflags::SetUsageMessage("git repo status for shell prompt, written in C++");
  gflags::SetVersionString("0.0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // args.hpp parser {{{
  // args::ArgumentParser parser(
  //     "git repo status for shell prompt, written in C++");
  // parser.Prog(argv[0]);
  // args::HelpFlag help(parser, "help", "Display this help menu and exit",
  //                     {'h', "help"});
  // args::CompletionFlag completion(parser, {"complete"});
  // args::Flag debug(parser, "debug", "display log messages", {'d', "debug"});
  //
  // try {
  //   parser.ParseCLI(argc, argv);
  // } catch (const args::Completion &e) {
  //   std::cout << e.what();
  //   return 0;
  // } catch (const args::Help &) {
  //   std::cout << parser;
  //   return 0;
  // } catch (const args::ParseError &e) {
  //   std::cerr << e.what() << std::endl;
  //   std::cerr << parser;
  //   return 1;
  // }
  //
  // if (args::get(debug)) {
  //   google::SetStderrLogging(google::INFO);
  // }
  // }}}

  Options *opts;
  Repo *ri;
  RepoArea *unstaged;
  RepoArea *staged;

  opts = new Options;
  ri = new Repo;
  unstaged = new RepoArea;
  staged = new RepoArea;

  // opts->format = "%g (%b@%c) %m %n %a %d %%";
  opts->format = FLAGS_format;
  opts->debug_print = true;
  ri->Unstaged = *unstaged;
  ri->Staged = *staged;

  const std::string gitStatus = Run("git status --porcelain=2 --branch");
  std::vector<std::string> statusLines = split(gitStatus, '\n');
  for (int i = 0; i < statusLines.size(); i++) {
    ri->parseGitStatus(statusLines[i]);
  }

  VLOG(1) << ri;

  parseFmtStr(opts);

  VLOG(1) << opts;

  VLOG(2) << "All command line flags:\n"
          << gflags::CommandlineFlagsIntoString() << std::endl;
  std::cout << "Format string: " << FLAGS_format << std::endl;

  delete opts;
  delete ri;
  delete unstaged;
  delete staged;

  gflags::ShutDownCommandLineFlags();
  return 0;
}
