#ifndef REPO_H
#define REPO_H

#include <iostream>
#include <string>

class RepoArea
{
  public:
    const char* MODIFIED_GLYPH = "Δ";
    int modified, added, deleted, renamed, copied;
    RepoArea()
    {
        modified = 0;
        added = 0;
        deleted = 0;
        renamed = 0;
        copied = 0;
    }
    friend std::ostream& operator<<(std::ostream&, RepoArea*);
    void debug(void);
    std::string print(void);
    void parseModified(const char&);
    bool hasChanged(void);
    std::string formatModified(bool);
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
    friend std::ostream& operator<<(std::ostream&, Repo*);
    void debug(void);
    std::string print(void);
    void parseBranch(const std::string&);
    void parseTrackedFile(const std::string&);
    void parseGitStatus(const std::string&);
    void parseGitDiff(const std::string&);
    std::string formatAheadBehind(bool);
    std::string formatBranch(void);
    std::string formatCommit(size_t);
    std::string formatDiff(void);
    std::string formatStashed(bool);
    std::string formatUntracked(bool);
};

#endif // REPO_H
