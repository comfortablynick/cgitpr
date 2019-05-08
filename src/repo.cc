#include "common.h"
#include "repo.h"

#include <ext/alloc_traits.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

std::ostream& operator<<(std::ostream& out, RepoArea* obj)
{
    out << "  Added: " << obj->added << '\n'
        << "  Copied: " << obj->copied << '\n'
        << "  Deleted: " << obj->deleted << '\n'
        << "  Modified: " << obj->modified << '\n'
        << "  Renamed: " << obj->renamed << '\n'
        << "  Dirty: " << obj->hasChanged() << '\n';
    return out;
}

void RepoArea::parseModified(const char& ltr)
{
    // count 'typechange' as a modified file
    if (ltr == 'M' || ltr == 'T') ++modified;
    if (ltr == 'A') ++added;
    if (ltr == 'D') ++deleted;
    if (ltr == 'R') ++renamed;
    if (ltr == 'C') ++copied;
}

bool RepoArea::hasChanged() const { return (added + copied + deleted + modified + renamed) != 0; }

std::ostream& operator<<(std::ostream& out, Repo* obj)
{
    out << "Repo:\n"
        << "Git Dir: " << obj->gitDir << '\n'
        << "Branch: " << obj->branch << '\n'
        << "Commit: " << obj->commit << '\n'
        << "Upstream: " << obj->upstream << '\n'
        << "Stashed: " << obj->stashed << '\n'
        << "Ahead: " << obj->ahead << '\n'
        << "Behind: " << obj->behind << '\n'
        << "Untracked: " << obj->untracked << '\n'
        << "Unmerged: " << obj->unmerged << '\n'
        << "Insertions: " << obj->insertions << '\n'
        << "Deletions: " << obj->deletions << '\n'
        << "Staged:\n"
        << obj->Staged << "Unstaged:\n"
        << obj->Unstaged;
    return out;
}

// Format modified file indicator and count
const std::string RepoArea::formatModified(bool indicators_only) const
{
    std::ostringstream ss;
    if (hasChanged()) {
        ss << Ansi::setFg(Color::red) << MODIFIED_GLYPH;
        if (!indicators_only) ss << modified;
        ss << Ansi::reset();
    }
    return ss.str();
}

// Parse branch details from `git status` string.
// @param str Git command output
void Repo::parseBranch(const std::string& str)
{
    const auto words = split(str, ' ');
    if (words[1] == "branch.oid") {
        commit = words[2];
    }
    if (words[1] == "branch.head") {
        branch = words[2];
    }
    if (words[1] == "branch.upstream") {
        upstream = words[2];
    }
    if (words[1] == "branch.ab") {
        ahead = stoi(words[2].substr(1));
        behind = stoi(words[3].substr(1));
    }
}


// Parse modified file details (Unstaged or Staged).
// @param str Git command output
void Repo::parseTrackedFile(const std::string& str)
{
    const auto words = split(str, ' ');
    Staged->parseModified(words[1][0]);
    Unstaged->parseModified(words[1][1]);
}

// Send `git status` lines to correct function to be parsed.
// @param str Git command output
void Repo::parseGitStatus(const std::string& str)
{
    auto words = split(str, ' ');
    if (words[0] == "#") parseBranch(str);
    if (words[0] == "1" || words[0] == "2") parseTrackedFile(str);
    if (words[0] == "u") ++unmerged;
    if (words[0] == "?") ++untracked;
}

// Parse `git diff --shortstat` to get inserted/deleted lines
// @param str Git command output
void Repo::parseGitDiff(const std::string& str)
{
    auto words = split(str, ' ');
    for (auto i = 0; i < words.size(); ++i) {
        if (words[i].find('+') != std::string::npos) insertions = std::stoi(words[i - 1]);
        if (words[i].find('-') != std::string::npos) deletions = std::stoi(words[i - 1]);
    }
}

// Format output of ahead/behind data
// @param indicators_only Show symbols only, not numbers
const std::string Repo::formatAheadBehind(bool indicators_only) const
{
    std::stringstream ss;
    if (ahead > 0) {
        ss << AHEAD_GLYPH;
        if (!indicators_only) ss << ahead;
    }
    if (behind > 0) {
        ss << BEHIND_GLYPH;
        if (!indicators_only) ss << behind;
    }
    return ss.str();
}

// Format output of branch
const std::string Repo::formatBranch() const
{
    std::ostringstream ss;
    ss << Ansi::setFg(Color::blue) << branch << Ansi::reset();
    return ss.str();
}

// Format commit hash
// @param string_len Length of commit hash representation
const std::string Repo::formatCommit(size_t str_len) const
{
    std::stringstream ss;
    ss << Ansi::setBg(Color::green) << Ansi::setFg(Color::black) << commit.substr(0, str_len)
       << Ansi::reset();
    return ss.str();
}

// Format `diff numstat` details
// untracked
const std::string Repo::formatDiff() const
{
    std::stringstream ss;
    if (insertions > 0) {
        ss << '+' << insertions;
        if (deletions > 0) {
            ss << '/';
        }
    }
    if (deletions > 0) {
        ss << '-' << deletions;
    }
    return ss.str();
}

// Format stash count/icon
const std::string Repo::formatStashed(bool indicators_only) const
{
    std::stringstream ss;
    if (stashed > 0) {
        ss << STASH_GLYPH;
        if (!indicators_only) ss << stashed;
    }
    return ss.str();
}

// Format untracked files count/icon
const std::string Repo::formatUntracked(bool indicators_only) const
{
    std::stringstream ss;
    if (untracked > 0) {
        ss << Ansi::setFg(Color::gray) << UNTRACKED_GLYPH;
        if (!indicators_only) ss << untracked;
        ss << Ansi::reset();
    }
    return ss.str();
}
