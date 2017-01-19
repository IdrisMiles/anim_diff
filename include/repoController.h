#ifndef REPOCONTROLLER_H
#define REPOCONTROLLER_H

#include <memory>
#include <string>

class RevisionNode;
class RevisionDiff;

class RepoController
{
public:
    RepoController();
    ~RepoController();

    std::shared_ptr<RevisionNode> loadMainNode(std::string _path);
    std::shared_ptr<RevisionNode> loadCompareNode(std::string _path);
    std::shared_ptr<RevisionNode> loadParentNode(std::string _path);

    std::shared_ptr<RevisionDiff> getDiff();
    void getMerge();

private:
    std::shared_ptr<RevisionNode> m_mainNode;
    std::shared_ptr<RevisionNode> m_compareNode;
    std::shared_ptr<RevisionNode> m_parentNode;        

};

#endif
