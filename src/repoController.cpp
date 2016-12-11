#include "repoController.h"
#include "revisionNode.h"
#include "revisionDiff.h"
#include "revisionUtils.h"

#include <iostream>

RepoController::RepoController()
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
        m_diff = std::make_shared<RevisionDiff>(diff);
    }
    catch(const std::string& ex) 
    {
        std::cout << ex << "\n";
        // return an empty diff
        m_diff = std::shared_ptr<RevisionDiff>(new RevisionDiff(m_mainNode, m_compareNode));
    }

    return m_diff;
}

void RepoController::exportDiff()
{
    // Create a revision node from the revisionDiff
    RevisionNode node = RevisionUtils::getRevisionNodeForDiff(m_diff);

    // export the node to file
}