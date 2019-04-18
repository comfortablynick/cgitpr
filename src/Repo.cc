#include "Repo.h"
#include "Utils.h"
#include <easylogging++.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::ostream& operator<<(std::ostream& out, RepoArea* obj)
{
    out << "  Added: " << obj->added << '\n';
    out << "  Copied: " << obj->copied << '\n';
    out << "  Deleted: " << obj->deleted << '\n';
    out << "  Modified: " << obj->modified << '\n';
    out << "  Renamed: " << obj->renamed << '\n';
    out << "  Dirty: " << obj->hasChanged() << '\n';
    return out;
}

void RepoArea::debug() { std::cerr << this; }

std::string RepoArea::print()
{
    std::stringstream ss;
    ss << this;
    return ss.str();
}

void RepoArea::parseModified(const char& ltr)
{
    if (ltr == 'M') {
        modified++;
    }
    if (ltr == 'A') {
        added++;
    }
    if (ltr == 'D') {
        deleted++;
    }
    if (ltr == 'R') {
        renamed++;
    }
    if (ltr == 'C') {
        copied++;
    }
}

bool RepoArea::hasChanged()
{
    return this->added + this->copied + this->deleted + this->modified + this->renamed != 0;
}

el::Logger* console = el::Loggers::getLogger("");
std::ostream& operator<<(std::ostream& out, Repo* obj)
{
    out << "Repo:\n";
    out << "Git Dir: " << obj->gitDir << '\n';
    out << "Branch: " << obj->branch << '\n';
    out << "Commit: " << obj->commit << '\n';
    out << "Upstream: " << obj->upstream << '\n';
    out << "Stashed: " << obj->stashed << '\n';
    out << "Ahead: " << obj->ahead << '\n';
    out << "Behind: " << obj->behind << '\n';
    out << "Untracked: " << obj->untracked << '\n';
    out << "Unmerged: " << obj->unmerged << '\n';
    out << "Insertions: " << obj->insertions << '\n';
    out << "Deletions: " << obj->deletions << '\n';
    out << "Staged:\n";
    out << obj->Staged.print();
    out << "Unstaged:\n";
    out << obj->Unstaged.print();
    return out;
}

// Format modified file indicator and count
std::string RepoArea::formatModified(bool indicators_only)
{
    std::stringstream ss;
    if (this->hasChanged()) {
        ss << this->MODIFIED_GLYPH;
        if (!indicators_only) {
            ss << this->modified;
        }
    }
    return ss.str();
}

// Print self using << overload.
void Repo::debug(void) { std::cerr << this; }

// Print to stringstream using << overload.
std::string Repo::print(void)
{
    std::stringstream ss;
    ss << this;
    return ss.str();
}


// Parse branch details from `git status` string.
// @param str Git command output
void Repo::parseBranch(const std::string& str)
{
    std::vector<std::string> words = split(str, ' ');
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
    std::vector<std::string> words = split(str, ' ');
    Staged.parseModified(words[1][0]);
    Unstaged.parseModified(words[1][1]);
}

// Send `git status` lines to correct function to be parsed.
// @param str Git command output
void Repo::parseGitStatus(const std::string& str)
{
    std::vector<std::string> words = split(str, ' ');
    if (words[0] == "#") {
        parseBranch(str);
    }
    if (words[0] == "1" || words[0] == "2") {
        parseTrackedFile(str);
    }
    if (words[0] == "u") {
        unmerged++;
    }
    if (words[0] == "?") {
        untracked++;
    }
}

// Parse `git diff --shortstat` to get inserted/deleted lines
// @param str Git command output
void Repo::parseGitDiff(const std::string& str)
{
    std::vector<std::string> words = split(str, ' ');
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i].find('+') != std::string::npos) insertions = std::stoi(words[i - 1]);
        if (words[i].find('-') != std::string::npos) deletions = std::stoi(words[i - 1]);
    }
}

// Format output of ahead/behind data
// @param indicators_only Show symbols only, not numbers
std::string Repo::formatAheadBehind(bool indicators_only)
{
    std::stringstream ss;
    if (this->ahead > 0) {
        ss << this->AHEAD_GLYPH;
        if (!indicators_only) {
            ss << this->ahead;
        }
    }
    if (this->behind > 0) {
        ss << this->BEHIND_GLYPH;
        if (!indicators_only) {
            ss << this->behind;
        }
    }
    return ss.str();
}

// Format output of branch
std::string Repo::formatBranch() { return this->branch; }

// Format commit hash
// @param string_len Length of commit hash representation
std::string Repo::formatCommit(size_t str_len) { return this->commit.substr(0, str_len); }

// Format `diff numstat` details
std::string Repo::formatDiff()
{
    std::stringstream ss;
    if (this->insertions > 0) {
        ss << "+" << this->insertions;
        if (this->deletions > 0) {
            ss << "/";
        }
    }
    if (this->deletions > 0) {
        ss << "-" << this->deletions;
    }
    return ss.str();
}

// Format stash count/icon
std::string Repo::formatStashed(bool indicators_only)
{
    std::stringstream ss;
    if (this->stashed > 0) {
        ss << this->STASH_GLYPH;
        if (!indicators_only) {
            ss << this->stashed;
        }
    }
    return ss.str();
}

// Format untracked files count/icon
std::string Repo::formatUntracked(bool indicators_only)
{
    std::stringstream ss;
    if (this->untracked > 0) {
        ss << this->UNTRACKED_GLYPH;
        if (!indicators_only) {
            ss << this->untracked;
        }
    }
    return ss.str();
}
