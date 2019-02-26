#include <iostream>
#include <string>

class Options {
public:
  bool debug_print, show_ahead_behind, show_branch, show_branch_glyph,
      show_commit, show_diff, show_remote, show_stashed, show_staged_modified,
      show_unstaged_modified, show_untracked, show_vcs;
  std::string format;
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
  friend std::ostream &operator<<(std::ostream &out, Options *obj);
  void debug(void);
  std::string print(void);
};
