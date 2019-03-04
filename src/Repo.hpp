#ifndef REPO_H
#define REPO_H

#include "RepoArea.hpp"
#include <iostream>
#include <string>

class Repo
{
  private:
    static Repo* instance;
    Repo()
    {
        stashed = 0;
        ahead = 0;
        behind = 0;
        untracked = 0;
        unmerged = 0;
        insertions = 0;
        deletions = 0;
        Unstaged = RepoArea();
        Staged = RepoArea();
    };

  public:
    static Repo* getInstance()
    {
        if (instance == 0) instance = new Repo();
        return instance;
    }
    std::string gitDir, branch, commit, remote, upstream;
    int stashed, ahead, behind, untracked, unmerged, insertions, deletions;
    RepoArea Unstaged, Staged;
    friend std::ostream& operator<<(std::ostream& out, Repo* obj);
    void debug(void);
    std::string print(void);
    void parseBranch(const std::string& str);
    void parseTrackedFile(const std::string& str);
    void parseGitStatus(const std::string& str);
    void parseGitDiff(const std::string& str);
};

#endif // REPO_H
