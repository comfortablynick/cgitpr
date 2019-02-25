#include "Repo.hpp"
#include "Utils.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void Repo::debug(void) {
  cout << "Working Dir: " << workingDir << '\n';
  cout << "Git Dir: " << gitDir << '\n';
  cout << "Branch: " << branch << '\n';
  cout << "Commit: " << commit << '\n';
  cout << "Upstream: " << upstream << '\n';
  cout << "Stashed: " << stashed << '\n';
  cout << "Ahead: " << ahead << '\n';
  cout << "Behind: " << behind << '\n';
  cout << "Untracked: " << untracked << '\n';
  cout << "Unmerged: " << unmerged << '\n';
  cout << "Insertions: " << insertions << '\n';
  cout << "Deletions: " << deletions << '\n';
  cout << "\nStaged:" << '\n';
  Staged.debug();
  cout << "\nUnstaged:" << '\n';
  Unstaged.debug();
}

void Repo::parseBranch(const string &str) {
  vector<string> words = split(str, ' ');
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

void Repo::parseTrackedFile(const string &str) {
  vector<string> words = split(str, ' ');
  Staged.parseModified(words[1][0]);
  Unstaged.parseModified(words[1][1]);
}

void Repo::parseGitStatus(const string &str) {
  vector<string> words = split(str, ' ');
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
