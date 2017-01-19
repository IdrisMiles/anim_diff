#include "repoController.h"
#include "revisionNode.h"
#include "revisionDiff.h"
#include "revisionUtils.h"
#include "revisionMerge.h"

#include <iostream>

RepoController::RepoController()
{
}

RepoController::~RepoController()
{

}

std::shared_ptr<RevisionNode> RepoController::loadParentNode(std::string _path)
{
    m_parentNode.reset(new RevisionNode());
    m_parentNode->LoadModel(_path);
    return m_parentNode;
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

std::shared_ptr<RevisionMerge> RepoController::getMerge()
{
    try 
    {
        // get both diffs of the children and master
        RevisionDiff mainDiff = RevisionUtils::getRevisionDiff(m_parentNode, m_mainNode);
        RevisionDiff compDiff = RevisionUtils::getRevisionDiff(m_parentNode, m_compareNode);

        // do something with these diffs. 
        // diff the diffs! crazy! mental!
        RevisionMerge merge = RevisionUtils::getRevisionMerge(mainDiff, compDiff, m_parentNode);
        return std::make_shared<RevisionMerge>(merge);

    }
    catch(const std::string& ex) 
    {
        std::cout << ex << "\n";
        return std::shared_ptr<RevisionMerge>(new RevisionMerge(m_mainNode, m_compareNode, m_parentNode));
    }
}