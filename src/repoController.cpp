#include "repoController.h"
#include "revisionNode.h"
#include "revisionDiff.h"
#include "revisionUtils.h"

#include <iostream>

RepoController::RepoController()
:
    m_mainNode(),
    m_compareNode()
{

}

RepoController::~RepoController()
{
}

std::shared_ptr<RevisionNode> RepoController::loadMainNode(std::string _path)
{
    m_mainNode.reset(new RevisionNode());
    m_mainNode->LoadModel(_path);
    return m_mainNode;
}

std::shared_ptr<RevisionNode> RepoController::loadCompareNode(std::string _path)
{
    m_compareNode.reset(new RevisionNode());
    m_compareNode->LoadModel(_path);
    return m_compareNode;
}

std::shared_ptr<RevisionDiff> RepoController::getDiff()
{
    try 
    {
        RevisionDiff diff = RevisionUtils::getRevisionDiff(m_mainNode, m_compareNode);
        return std::make_shared<RevisionDiff>(diff);
    }
    catch(const std::string& ex) 
    {
        std::cout << ex << "\n";
        // return an empty diff
        return std::shared_ptr<RevisionDiff>(new RevisionDiff(m_mainNode, m_compareNode));
    }
}