#ifndef REPO_H
#define REPO_H

#include "RepoArea.h"
#include <string>

using namespace std;

class Repo {
public:
  // string workingDir;
  // string gitDir;
  // string branch;
  // string commit;
  // string remote;
  // string upstream;
  string workingDir, gitDir, branch, commit, remote, upstream;
  int stashed, ahead, behind, untracked, unmerged, insertions, deletions;
  // int stashed{0};
  // int ahead{0};
  // int behind{0};
  // int untracked{0};
  // int unmerged{0};
  // int insertions{0};
  // int deletions{0};
  // RepoArea Unstaged;
  // RepoArea Staged;
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
  }
  void debug(void);
  void parseBranch(const string &str);
  void parseTrackedFile(const string &str);
  void parseGitStatus(const string &str);
};

#endif // REPO_H
