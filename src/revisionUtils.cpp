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
    std::shared_ptr<ModelRig> masterRig = _master->m_model->m_rig;
    std::shared_ptr<ModelRig> branchRig = _branch->m_model->m_rig;

    if(masterRig->hasAnimation() && branchRig->hasAnimation())
    {   
        //TODO check to see if rigs match


        //just look at the first animation for now
        double masterTicks = masterRig->m_ticks;
        double masterDuration = masterRig->m_duration;

        double branchTicks = branchRig->m_ticks;
        double branchDuration = branchRig->m_duration;

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


DiffRig RevisionUtils::getAnimDiff(std::shared_ptr<ModelRig>, std::shared_ptr<ModelRig> branch)
{
    //TODO 
    // iterate through bones in each rig and compare 

    return DiffRig();
}

BoneAnimDiff RevisionUtils::getBoneDiff(BoneAnim master, BoneAnim branch)
{
    //TODO
    // iterate throught boneAnims and compare
    return BoneAnimDiff();
}

BoneAnimDelta RevisionUtils::getBoneAnimDelta(BoneAnim master, BoneAnim branch)
{
    // TODO
    // compare individual values and store the deltas
    return BoneAnimDelta();
}
