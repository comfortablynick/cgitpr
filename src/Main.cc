#include "Options.hpp"
#include "Repo.hpp"
#include "Utils.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
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
      case '%':
        break;
      default:
        fprintf(stderr, "error: token '%c' not recognized\n", fmt[i]);
        exit(1);
      }
    }
  }
}

int main() {
  Options *opts;
  Repo *ri;
  RepoArea *unstaged;
  RepoArea *staged;

  opts = new Options;
  ri = new Repo;
  unstaged = new RepoArea;
  staged = new RepoArea;

  opts->format = "%g (%b@%c) %m %n %a %d %%";
  opts->debug_print = true;
  ri->Unstaged = *unstaged;
  ri->Staged = *staged;

  const std::string gitStatus = Run("git status --porcelain=2 --branch");
  std::vector<std::string> statusLines = split(gitStatus, '\n');
  for (int i = 0; i < statusLines.size(); i++) {
    ri->parseGitStatus(statusLines[i]);
  }

  // logger
  auto clog = spdlog::stderr_color_mt("cgitpr");
  clog->set_level(spdlog::level::debug);
  clog->info("Console log level: {}",
             spdlog::level::to_string_view(clog->level()));
  clog->info(ri->print());
  parseFmtStr(opts);
  clog->info(opts->print());

  delete opts;
  delete ri;
  delete unstaged;
  delete staged;
  return 0;
}
