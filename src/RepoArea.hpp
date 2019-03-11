#ifndef REPO_AREA_H
#define REPO_AREA_H
#include <iostream>

class RepoArea
{
  public:
    int modified, added, deleted, renamed, copied;
    RepoArea()
    {
        modified = 0;
        added = 0;
        deleted = 0;
        renamed = 0;
        copied = 0;
    }
    friend std::ostream& operator<<(std::ostream& out, RepoArea* obj);
    void debug(void);
    std::string print(void);
    void parseModified(const char& ltr);
    bool hasChanged(void);
};

#endif // REPO_AREA_H
