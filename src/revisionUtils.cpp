#include "revisionUtils.h"

#include "revisionNode.h"
#include <assimp/scene.h>

#include <iostream>

RevisionUtils::RevisionUtils()
{

}

RevisionUtils::~RevisionUtils()
{

}

RevisionDiff RevisionUtils::getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
{   
    if(!_master && !_branch) throw std::string("null pointer: revision node");

    if(!_master->m_model && !_master->m_model) throw std::string("null pointer: no model loaded");
    
    // public members, so so bad and dirty raw pointers
    ModelRig masterRig = _master->m_model->getRig();
    ModelRig branchRig = _branch->m_model->getRig();

    if(masterRig.hasAnimation() && branchRig.hasAnimation())
    {   
        //just look at the first animation for now
        double master_ticksPerSecond = masterRig.getTicks();
        double master_animationDuration = masterRig.getDuration();

        double branch_ticksPerSecond = branchRig.getTicks();
        double branch_animationDuration = branchRig.getDuration();

        // Todo our ticks and duration calculations to match

        // Then diff

    }
    else
    {
        throw std::string("no animation present");
    }

    return RevisionDiff();
}

DiffRig RevisionUtils::diff(std::shared_ptr<ModelRig> master, std::shared_ptr<ModelRig> branch)
{
    return DiffRig();
}