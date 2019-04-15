#ifndef REPO_H
#define REPO_H

#include <iostream>
#include <string>

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
    const char* BRANCH_GLYPH = "";
    const char* MODIFIED_GLYPH = "Δ";
    const char* UNTRACKED_GLYPH = "…";
    const char* AHEAD_GLYPH = "↑";
    const char* BEHIND_GLYPH = "↓";
    const char STASH_GLYPH = '$';
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
    std::string formatAheadBehind(bool indicatorsOnly);
};

#endif // REPO_H
