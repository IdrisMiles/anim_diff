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
        double masterTicks = masterRig.getTicks();
        double masterDuration = masterRig.getDuration();

        double branchTicks = branchRig.getTicks();
        double branchDuration = branchRig.getDuration();

        // Todo our ticks and duration calculations to match
        if((masterTicks != branchTicks) || 
            (masterDuration != branchDuration))
        {
            // match some ticks and duration
        }

        // Then diff the animation info on a per tick basis for their durations
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