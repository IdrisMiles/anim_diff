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
    BoneAnimDiff boneDiff;

    //TODO
    // iterate through data and compare
    boneDiff.m_posAnimDeltas = getPositionDiffs(master.m_posAnim, branch.m_posAnim);

    //First lets do pos
    // GLM mix for vector interp and GLM slurp for quats


    //Now Rotate


    //Now scale

    return boneDiff;
}

std::vector<PosAnim> RevisionUtils::getPositionDiffs(std::vector<PosAnim>& master, std::vector<PosAnim>& branch)
{
    std::vector<PosAnim> PosAnimDiffs;

    auto mItr = master.begin();
    auto bItr = branch.begin();

    while((mItr != master.end()) && (bItr != branch.end()))
    {

        if(mItr == master.end())
        {
            // just log branch because master is finished
            PosAnim posDiff;
            posDiff.time = (*bItr).time;
            posDiff.pos = glm::vec3(0,0,0);
            PosAnimDiffs.push_back(posDiff);
        }
        else if(bItr == branch.end())
        {
            PosAnim posDiff;
            posDiff.time = (*mItr).time;
            posDiff.pos = glm::vec3(0,0,0);
            PosAnimDiffs.push_back(posDiff);
        }

        // we have a time match
        if((*mItr).time == (*bItr).time)
        {
            // we have a match lets compare the positions
            glm::vec3 mPos = (*mItr).pos;
            glm::vec3 bPos = (*bItr).pos;

            PosAnim posDiff;

            posDiff.pos.x = mPos.x - bPos.x;
            posDiff.pos.y = mPos.y - bPos.y;
            posDiff.pos.z = mPos.z - bPos.z;

            posDiff.time = (*mItr).time; 

            // add to the things
            PosAnimDiffs.push_back(posDiff);

            mItr++;
            bItr++;
        }
        // check if one time is behind the other
        else if((*mItr).time < (*bItr).time)
        {
            // no matching times so make one 
            // calc interpolate amount
            auto PrevBItr = std::prev(bItr);
            float interTime = ((*bItr).time - (*PrevBItr).time) / ((*mItr).time - (*PrevBItr).time);

            glm::vec3 mPos = (*mItr).pos;
            glm::vec3 bPos = glm::mix((*bItr).pos , (*PrevBItr).pos, interTime);

            PosAnim posDiff;

            posDiff.pos.x = mPos.x - bPos.x;
            posDiff.pos.y = mPos.y - bPos.y;
            posDiff.pos.z = mPos.z - bPos.z;

            posDiff.time = (*mItr).time; 

            // add to the things
            PosAnimDiffs.push_back(posDiff);
            mItr++;
        }
        // now the branch is more than 
        else
        {
            auto PrevMItr = std::prev(mItr);
            float interTime = ((*mItr).time - (*PrevMItr).time) / ((*bItr).time - (*PrevMItr).time);

            glm::vec3 mPos = glm::mix((*mItr).pos , (*PrevMItr).pos, interTime);
            glm::vec3 bPos = (*bItr).pos;

            PosAnim posDiff;

            posDiff.pos.x = mPos.x - bPos.x;
            posDiff.pos.y = mPos.y - bPos.y;
            posDiff.pos.z = mPos.z - bPos.z;

            posDiff.time = (*mItr).time; 

            // add to the things
            PosAnimDiffs.push_back(posDiff);
            bItr++;
        }
    }

    return PosAnimDiffs;
}

std::vector<RotAnim> RevisionUtils::getRotationDiffs(std::vector<RotAnim>& master, std::vector<RotAnim>& branch)
{

}
    
std::vector<ScaleAnim> RevisionUtils::getScaleDiffs(std::vector<ScaleAnim>& master, std::vector<ScaleAnim>& branch)
{

}
