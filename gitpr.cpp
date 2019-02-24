#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

std::vector<string> split(const std::string &str, char delim = ' ') {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);
  while (std::getline(tokenStream, token, delim)) {
    tokens.push_back(token);
  }
  return tokens;
}

class RepoArea {
public:
  int modified;
  int added;
  int deleted;
  int renamed;
  int copied;
};

class Repo {
public:
  std::string workingDir;
  std::string gitDir;
  std::string branch;
  std::string commit;
  std::string remote;
  std::string upstream;
  int stashed;
  int ahead;
  int behind;
  int untracked;
  int unmerged;
  int insertions;
  int deletions;
  RepoArea Unstaged;
  RepoArea Staged;

  void debug(void);
  void parseBranch(const std::string &str);
  void parseGitStatus(const std::string &str);
};

void Repo::debug(void) {
  std::cout << "Working Dir: " << workingDir << std::endl;
  std::cout << "Git Dir: " << gitDir << std::endl;
  std::cout << "Branch: " << branch << std::endl;
  std::cout << "Commit: " << commit << std::endl;
  std::cout << "Upstream: " << upstream << std::endl;
  std::cout << "Stashed: " << stashed << std::endl;
  std::cout << "Ahead: " << ahead << std::endl;
  std::cout << "Behind: " << behind << std::endl;
  std::cout << "Untracked: " << untracked << std::endl;
  std::cout << "Unmerged: " << unmerged << std::endl;
  std::cout << "Insertions: " << insertions << std::endl;
  std::cout << "Deletions: " << deletions << std::endl;
}

void Repo::parseBranch(const std::string &str) {
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
  // if (words[1] == "branch.ab") {
  //     // Need to parse +n -n to int
  // }
}

void Repo::parseGitStatus(const std::string &str) {
  std::vector<std::string> words = split(str, ' ');
  if (words[0] == "#") {
    parseBranch(str);
  }
}

std::string Run(std::string cmd) {
  std::string data;
  FILE *stream;
  const int max_buffer = 256;
  char buffer[max_buffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");
  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL)
        data.append(buffer);
    pclose(stream);
  }
  return data;
}

int main() {
  Repo ri;
  std::string gitStatus = Run("git status --porcelain=2 --branch");
  std::vector<std::string> statusLines = split(gitStatus, '\n');
  for (int i = 0; i < statusLines.size(); i++) {
    ri.parseGitStatus(statusLines[i]);
  }
  ri.debug();
  return 0;
}
