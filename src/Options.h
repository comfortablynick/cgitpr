#include <iostream>
#include <sstream>
#include <string>

class Options
{
  public:
    bool debug_print, debug_quiet, indicators_only, show_ahead_behind, show_branch,
        show_branch_glyph, show_commit, show_diff, show_remote, show_stashed, show_staged_modified,
        show_unstaged_modified, show_untracked, show_vcs, no_color, simple_mode;
    std::string format, dir;
    int verbosity;
    Options()
        : debug_print(false), debug_quiet(false), indicators_only(false), show_ahead_behind(false),
          show_branch(false), show_branch_glyph(false), show_commit(false), show_diff(false),
          show_remote(false), show_stashed(false), show_staged_modified(false),
          show_unstaged_modified(false), show_untracked(false), show_vcs(false), no_color(false),
          simple_mode(false), format("%g %b@%c %a %m %d %s %u %t"), dir("."), verbosity(0)
    {}

    friend std::ostream& operator<<(std::ostream& out, Options* obj)
    {
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
        out << "  Debug print: " << obj->debug_print << '\n';
        out << "  Debug quiet: " << obj->debug_quiet << '\n';
        out << "  No color: " << obj->no_color << '\n';
        out << "  Simple mode: " << obj->simple_mode << '\n';
        out << "  Indicators only: " << obj->indicators_only << '\n';
        out << "  Format: " << obj->format << '\n';
        out << "  Dir: " << obj->dir << '\n';
        out << "  Verbosity: " << obj->verbosity << '\n';
        return out;
    }
};
