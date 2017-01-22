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

RevisionMerge RevisionUtils::getRevisionMerge(RevisionDiff _diffA, 
                                        RevisionDiff _diffB, 
                                        std::shared_ptr<RevisionNode> _parentNode)
{
    // we create a merge rig
    RevisionMerge merge(_diffA.getMasterNode(), _diffA.getBranchNode(), _parentNode);
    
    MergeRig rig;

    //find longest duration
    double masterDuration = _diffA.getDiffRig().m_duration;
    double branchDuration = _diffB.getDiffRig().m_duration;
    double parentDuration = _parentNode->m_model->m_rig->m_duration;
    
    double largest;
    if( masterDuration > branchDuration && masterDuration > parentDuration)
    {
        largest = masterDuration;
    }
    else if( branchDuration > parentDuration && branchDuration > masterDuration) 
    {
        largest = branchDuration;
    }
    else
    {
        largest = parentDuration;
    }

    rig.m_duration = largest;
    
    // may aswell take parent ticks
    rig.m_ticks = _parentNode->m_model->m_rig->m_ticks;
 
    // we will now do an XOR with these diffs.
    getAnimMerge(_diffA.getDiffRig(), _diffB.getDiffRig(), rig);
    
    merge.setMergeRig(rig);

    return merge;
}
