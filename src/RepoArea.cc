#include "RepoArea.h"
#include <iostream>
#include <string>

using namespace std;

void RepoArea::debug() {
  cout << "  Added: " << added << '\n';
  cout << "  Copied: " << copied << '\n';
  cout << "  Deleted: " << deleted << '\n';
  cout << "  Modified: " << modified << '\n';
  cout << "  Renamed: " << renamed << endl;
}

void RepoArea::parseModified(const char &ltr) {
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
