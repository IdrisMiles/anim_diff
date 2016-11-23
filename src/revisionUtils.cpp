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
    RevisionDiff diff(_master, _branch);

    if(!_master && !_branch) throw std::string("null pointer: revision node");

    if(!_master->m_model && !_branch->m_model) throw std::string("null pointer: no model loaded");
    
    if(_master->m_model->m_animExists && _branch->m_model->m_animExists)
    {   

        // public members, so so bad and dirty raw pointers
        ModelRig masterRig = _master->m_model->m_rig;
        ModelRig branchRig = _branch->m_model->m_rig;

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

void RevisionUtils::getAnimDiff(ModelRig master, ModelRig branch, DiffRig &outRig)
{
    // iterate through master bones
    for(auto bone : master.m_boneAnims)
    {
        // find matching one
        auto branchBone = branch.m_boneAnims.find(bone.first);

        if(branchBone != branch.m_boneAnims.end())
        {
            BoneAnimDiff boneDiff = getBoneDiff(bone.second, branchBone->second);
            outRig.m_boneAnimDiffs.insert({bone.first, boneDiff});
        }
        else
        {
            std::cout << "cannot find matching bone for: " << bone.first << "\n";
        }
    }
}

BoneAnimDiff RevisionUtils::getBoneDiff(BoneAnim master, BoneAnim branch)
{
    BoneAnimDiff BoneDiff;

    //TODO
    // iterate through data and compare 

    //First lets do pos
    // GLM mix for vector interp and GLM slurp for quats
    auto mItr = master.m_posAnim.begin();
    auto bItr = branch.m_posAnim.begin();

    while((mItr != master.m_posAnim.end()) && (bItr != branch.m_posAnim.end()))
    {

        //TODO checks for if one is at the end and th other isnt;

        // we have a time match
        if((*mItr).time == (*bItr).time)
        {
            // we have a match lets compare the positions
            glm::vec3 mPos = (*mItr).pos;
            glm::vec3 bPos = (*bItr).pos;

            glm::vec3 posDiff;

            posDiff.x = mPos.x - bPos.x;
            posDiff.y = mPos.y - bPos.y;
            posDiff.z = mPos.z - bPos.z;

            // add to the things
            boneDiff.m_posAnimDeltas.push_back(posDiff);

            mItr++;
            bItr++;
        }
        // check if one time is behind the other
        else if((*mItr).time < (*bItr).time)
        {
            mItr++;
        }
        // now the branch is more than 
        else
        {
            bItr++;
        }
    }

    //Now Rotate


    //Now scale

    return boneDiff;
}
