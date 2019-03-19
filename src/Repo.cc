#include "Repo.hpp"
#include "Utils.hpp"
#include <easylogging++.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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


/**
 * Print self using << overload.
 */
void Repo::debug(void) { std::cerr << this; }

/**
 * Print to stringstream using << overload.
 */
std::string Repo::print(void)
{
    std::stringstream ss;
    ss << this;
    return ss.str();
}

/**
 * Parse branch details from `git status` string.
 */
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

/**
 * Parse modified file details (Unstaged or Staged).
 */
void Repo::parseTrackedFile(const std::string& str)
{
    std::vector<std::string> words = split(str, ' ');
    Staged.parseModified(words[1][0]);
    Unstaged.parseModified(words[1][1]);
}

/**
 * Send `git status` lines to correct function to be parsed.
 */
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

/**
 * Parse `git diff --shortstat` to get inserted/deleted lines
 */
void Repo::parseGitDiff(const std::string& str)
{
    std::vector<std::string> words = split(str, ' ');
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i].find('+') != std::string::npos) insertions = std::stoi(words[i - 1]);
        if (words[i].find('-') != std::string::npos) deletions = std::stoi(words[i - 1]);
    }
}
