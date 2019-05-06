#ifndef REPO_H
#define REPO_H

#include <iostream>
#include <memory>
#include <stddef.h>
#include <string>

class RepoArea
{
  public:
    const char* const MODIFIED_GLYPH = "Δ";
    unsigned int modified, added, deleted, renamed, copied;
    RepoArea() : modified(0), added(0), deleted(0), renamed(0), copied(0) {}

    friend std::ostream& operator<<(std::ostream&, RepoArea*);
    void parseModified(const char&);
    bool hasChanged() const;
    const std::string formatModified(bool) const;
};

class Repo
{
  public:
    const char* const BRANCH_GLYPH = "";
    const char* const UNTRACKED_GLYPH = "…";
    const char* const AHEAD_GLYPH = "⇡";
    const char* const BEHIND_GLYPH = "⇣";
    const char STASH_GLYPH = '$';
    std::string gitDir, branch, commit, remote, upstream;
    unsigned int stashed, ahead, behind, untracked, unmerged, insertions, deletions;
    std::shared_ptr<RepoArea> Unstaged, Staged;
    Repo()
        : stashed(0), ahead(0), behind(0), untracked(0), unmerged(0), insertions(0), deletions(0),
          Unstaged(new RepoArea()), Staged(new RepoArea()){};

    friend std::ostream& operator<<(std::ostream&, Repo*);
    void parseBranch(const std::string&);
    void parseTrackedFile(const std::string&);
    void parseGitStatus(const std::string&);
    void parseGitDiff(const std::string&);
    const std::string formatAheadBehind(bool) const;
    const std::string formatBranch() const;
    const std::string formatCommit(size_t) const;
    const std::string formatDiff() const;
    const std::string formatStashed(bool) const;
    const std::string formatUntracked(bool) const;
};

#endif // REPO_H
