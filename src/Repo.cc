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

void Repo::debug(void) { std::cerr << this; }

std::string Repo::print(void)
{
    std::stringstream ss;
    ss << this;
    return ss.str();
}

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

void Repo::parseTrackedFile(const std::string& str)
{
    std::vector<std::string> words = split(str, ' ');
    Staged.parseModified(words[1][0]);
    Unstaged.parseModified(words[1][1]);
}

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

void Repo::parseGitDiff(const std::string& str)
{
    console->trace("%v", str);
    std::vector<std::string> words = split(str, ' ');

    VLOG(1) << words[0];
    for (auto word : words) console->info("%v", word);
}
