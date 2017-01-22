#ifndef REPOCONTROLLER_H
#define REPOCONTROLLER_H

#include <memory>
#include <string>

class RevisionNode;
class RevisionDiff;
class RevisionMerge;

class RepoController
{
public:
    RepoController();
    ~RepoController();

    std::shared_ptr<RevisionNode> loadMainNode(std::string _path);
    std::shared_ptr<RevisionNode> loadCompareNode(std::string _path);
    std::shared_ptr<RevisionNode> loadParentNode(std::string _path);

    std::shared_ptr<RevisionDiff> getDiff();
    std::shared_ptr<RevisionMerge> getMerge();

    void exportDiff();

private:
    std::shared_ptr<RevisionNode> m_mainNode;
    std::shared_ptr<RevisionNode> m_compareNode;
    std::shared_ptr<RevisionNode> m_parentNode;

    std::shared_ptr<RevisionDiff> m_diff;
    std::shared_ptr<RevisionMerge> m_merge;
};

#endif
