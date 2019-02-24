#ifndef REPO_AREA_H
#define REPO_AREA_H
#include <string>

using namespace std;

class RepoArea {
public:
  int modified, added, deleted, renamed, copied;
  RepoArea() {
    modified = 0;
    added = 0;
    deleted = 0;
    renamed = 0;
    copied = 0;
  }
  void debug();
  void parseModified(const char &ltr);
};

#endif // REPO_AREA_H
