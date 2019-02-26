#include "RepoArea.hpp"
#include <iostream>
#include <sstream>
#include <string>

std::ostream &operator<<(std::ostream &out, RepoArea *obj) {
  out << "  Added: " << obj->added << '\n';
  out << "  Copied: " << obj->copied << '\n';
  out << "  Deleted: " << obj->deleted << '\n';
  out << "  Modified: " << obj->modified << '\n';
  out << "  Renamed: " << obj->renamed << '\n';
  return out;
}

void RepoArea::debug() { std::cerr << this; }

std::string RepoArea::print() {
  std::stringstream ss;
  ss << this;
  return ss.str();
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
