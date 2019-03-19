#include "Options.hpp"
#include <iostream>
#include <sstream>

std::ostream& operator<<(std::ostream& out, Options* obj)
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
    out << "  Format: " << obj->format << '\n';
    out << "  Dir: " << obj->dir << '\n';
    return out;
}

void Options::debug()
{
    if (this->debug_print) {
        std::cerr << this;
    }
}

std::string Options::print()
{
    std::stringstream ss;
    ss << this;
    return ss.str();
}
