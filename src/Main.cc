#include "Repo.h"
#include "RepoArea.h"
#include "Utils.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
void parseFmtStr(const string &fmt) {
  cout << "Format string: " << fmt << "\n";

  for (string::size_type i = 0; i < fmt.length(); i++) {
    if (fmt[i] == '%') {
      i++;
      switch (fmt[i]) {
      case '\0': // end of string
        break;
      case 'a':
        break; // ahead/behind
      case 'g':
        cout << vformat("Found '%s'", "%g") << '\n';
      case 'n':
        cout << "Found %n" << '\n';
      default:
        cerr << "Error! Unrecognized format string element '" << fmt[i] << "'"
             << endl;
        return;
      }
    }
  }
}

int main() {
  Repo ri;
  RepoArea unstaged;
  RepoArea staged;
  ri.Unstaged = unstaged;
  string gitStatus = Run("git status --porcelain=2 --branch");
  vector<string> statusLines = split(gitStatus, '\n');
  for (int i = 0; i < statusLines.size(); i++) {
    ri.parseGitStatus(statusLines[i]);
  }
  ri.debug();
  const string fmt = "%g %b %m %a %d %x %%";
  parseFmtStr(fmt);
  return 0;
}
