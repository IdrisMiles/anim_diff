#include "revisionUtils.h"
#include "revisionNode.h"
#include <iostream>

RevisionUtils::RevisionUtils()
: DiffFunctions()
{

}

RevisionUtils::~RevisionUtils()
{

}

RevisionDiff RevisionUtils::getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
{   
    RevisionDiff diff(_master, _branch);

    if(!_master && !_branch) throw std::string("null pointer: revision node");

    if(!_master->m_model && !_branch->m_model) throw std::string("null pointer: no model loaded");
    
    if(_master->m_model->m_animExists && _branch->m_model->m_animExists)
    {   

        // public members, so so bad and dirty raw pointers
        ModelRig masterRig = *_master->m_model->m_rig;
        ModelRig branchRig = *_branch->m_model->m_rig;

        //TODO check to see if rigs and bones match


        //create a DiffRig
        DiffRig diffRig;

        //just look at the first animation for now
        double masterTicks = masterRig.m_ticks;
        double masterDuration = masterRig.m_duration;

        double branchTicks = branchRig.m_ticks;
        double branchDuration = branchRig.m_duration;

        // use master tick speed, and duration is the larger of the two 
        diffRig.m_ticks = masterTicks;
        diffRig.m_duration = masterDuration > branchDuration ? masterDuration : branchDuration;

        // Then diff the animation info on a per tick basis for their durations
        getAnimDiff(masterRig, branchRig, diffRig);

        //return 
        diff.setDiffRig(diffRig);
        return diff;
    }
    else
    {
        throw std::string("no animation present");
    }

    return diff;
}

RevisionNode RevisionUtils::getRevisionNodeForDiff(std::shared_ptr<RevisionDiff> _diff)
{
    RevisionNode node;

    node.m_model = std::make_shared<Model>(getModelFromDiff(_diff));

    return node;
}

Model RevisionUtils::getModelFromDiff(std::shared_ptr<RevisionDiff> _diff)
{
    //TODO
    return Model();
}
