#ifndef REPO_H
#define REPO_H

#include "RepoArea.hpp"
#include <string>

class Repo {
public:
  std::string workingDir, gitDir, branch, commit, remote, upstream;
  int stashed, ahead, behind, untracked, unmerged, insertions, deletions;
  RepoArea Unstaged, Staged;

  Repo() {
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
  void debug(void);
  void parseBranch(const std::string &str);
  void parseTrackedFile(const std::string &str);
  void parseGitStatus(const std::string &str);
};

#endif // REPO_H
