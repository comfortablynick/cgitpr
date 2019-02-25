#include "Repo.hpp"
#include "Utils.hpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Options {
public:
  bool debug_print, show_ahead_behind, show_branch, show_branch_glyph,
      show_commit, show_diff, show_remote, show_stashed, show_staged_modified,
      show_unstaged_modified, show_untracked, show_vcs;
  string format;
  Options() {
    debug_print = false;
    show_ahead_behind = false;
    show_branch = false;
    show_branch_glyph = false;
    show_commit = false;
    show_diff = false;
    show_remote = false;
    show_stashed = false;
    show_staged_modified = false;
    show_unstaged_modified = false;
    show_untracked = false;
    show_vcs = false;
  }
  friend ostream &operator<<(ostream &out, Options *obj) {
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
    return out;
  }
};

void parseFmtStr(Options *opts) {
  const string &fmt = opts->format;
  cout << "Format string: " << fmt << "\n";

  for (string::size_type i = 0; i < fmt.length(); i++) {
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
        cerr << "Error! Unrecognized format string element '" << fmt[i] << "'"
             << endl;
        return;
      }
    }
  }
  cout << '\n' << opts << endl;
}

int main() {
  Repo ri;
  RepoArea unstaged;
  RepoArea staged;
  ri.Unstaged = unstaged;
  string gitStatus = Run("git status --porcelain=2 --branch");
  vector<string> statusLines = split(gitStatus, '\n');
  for (int i = 0; i < statusLines.size(); i++) {
    ri.parseGitStatus(statusLines[i]);
  }
  ri.debug();
  const string fmt = "%g %b %m %n %a %d %%";
  Options *opts;
  opts = new Options;
  opts->format = fmt;
  parseFmtStr(opts);
  delete opts;
  return 0;
}
