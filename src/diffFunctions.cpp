#include "diffFunctions.h"
#include <iostream>

void DiffFunctions::getAnimDiff(ModelRig master, ModelRig branch, DiffRig &outRig)
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

BoneAnimDiff DiffFunctions::getBoneDiff(BoneAnim master, BoneAnim branch)
{
    BoneAnimDiff boneDiff;

    // get deltas for each item
    boneDiff.m_posAnimDeltas = getPositionDiffs(master.m_posAnim, branch.m_posAnim);

    boneDiff.m_rotAnimDeltas = getRotationDiffs(master.m_rotAnim, branch.m_rotAnim);

    boneDiff.m_scaleAnimDeltas = getScaleDiffs(master.m_scaleAnim, branch.m_scaleAnim);

    return boneDiff;
}

std::vector<PosAnim> DiffFunctions::getPositionDiffs(std::vector<PosAnim>& master, std::vector<PosAnim>& branch)
{
    std::vector<PosAnim> PosAnimDiffs;

    // lets use count instead of iterators
    unsigned int masterSize = master.size();
    unsigned int branchSize = branch.size();

    unsigned int mCount = 0;
    unsigned int bCount = 0;

    while(mCount < masterSize || bCount < branchSize)
    {
        if(mCount == masterSize)
        {
            // just log branch because master is finished
            PosAnim posDiff;
            posDiff.time = branch[bCount].time;
            posDiff.pos = glm::vec3(0,0,0);
            PosAnimDiffs.push_back(posDiff);
            bCount++;
        }
        else if(bCount == branchSize)
        {
            PosAnim posDiff;
            posDiff.time = master[mCount].time;
            posDiff.pos = glm::vec3(0,0,0);
            PosAnimDiffs.push_back(posDiff);
            mCount++;
        }
        else
        {
            // we have a time match
            if(master[mCount].time == branch[bCount].time)
            {
                // add to the things
                PosAnimDiffs.push_back(getPosDiff(master[mCount], branch[bCount]));

                mCount++;
                bCount++;
            }
            // check if one time is behind the other
            else if(master[mCount].time < branch[bCount].time)
            {
                // no matching times so make one 
                // calc interpolate amount
                unsigned int prevCount = bCount - 1;
                PosAnimDiffs.push_back(getPosDiff(master[mCount], branch[bCount], branch[prevCount]));
                mCount++;
            }
            // now the branch is more than 
            else
            {
                unsigned int prevCount = bCount - 1;
                PosAnimDiffs.push_back(getPosDiff(branch[bCount], master[mCount], master[prevCount]));
                bCount++;
            }
        }
    }

    return PosAnimDiffs;
}

PosAnim DiffFunctions::getPosDiff(PosAnim posA, PosAnim posB)
{
    glm::vec3 pos1 = posA.pos;
    glm::vec3 pos2 = posB.pos;

    PosAnim posDiff;

    posDiff.pos.x = pos2.x - pos1.x;
    posDiff.pos.y = pos2.y - pos1.y;
    posDiff.pos.z = pos2.z - pos1.z;

    posDiff.time = posA.time;
    
    return posDiff;
}

PosAnim DiffFunctions::getPosDiff(PosAnim posA, PosAnim posB1, PosAnim posB2)
{
    float interTime = (posA.time - posB2.time) / (posB1.time - posB2.time);

    glm::vec3 interPos = glm::mix(posB1.pos , posB2.pos, interTime);
    glm::vec3 pos = posA.pos;

    PosAnim posDiff;

    posDiff.pos.x = pos.x - interPos.x;
    posDiff.pos.y = pos.y - interPos.y;
    posDiff.pos.z = pos.z - interPos.z;

    posDiff.time = posA.time;

    return posDiff; 
}

std::vector<RotAnim> DiffFunctions::getRotationDiffs(std::vector<RotAnim>& master, std::vector<RotAnim>& branch)
{
    std::vector<RotAnim> rotAnimDiffs;

    // lets use count instead of iterators
    unsigned int masterSize = master.size();
    unsigned int branchSize = branch.size();

    unsigned int mCount = 0;
    unsigned int bCount = 0;

    while(mCount < masterSize || bCount < branchSize)
    {
        if(mCount == masterSize)
        {
            // just log branch because master is finished
            RotAnim rotDiff;
            rotDiff.time = branch[bCount].time;
            rotDiff.rot = glm::quat(0,0,0,0);
            rotAnimDiffs.push_back(rotDiff);
            bCount++;
        }
        else if(bCount == branchSize)
        {
            RotAnim rotDiff;
            rotDiff.time = master[mCount].time;
            rotDiff.rot = glm::quat(0,0,0,0);
            rotAnimDiffs.push_back(rotDiff);
            mCount++;
        }
        else
        {
            // we have a time match
            if(master[mCount].time == branch[bCount].time)
            {
                // add to the things
                rotAnimDiffs.push_back(getRotDiff(master[mCount] , branch[bCount]));

                mCount++;
                bCount++;
            }
            // check if one time is behind the other
            else if(master[mCount].time < branch[bCount].time)
            {
                // no matching times so make one 
                // calc interpolate amount
                unsigned int prevCount = bCount - 1;
                rotAnimDiffs.push_back(getRotDiff(master[mCount] , branch[bCount] , branch[prevCount]));
                mCount++;
            }
            // now the branch is more than 
            else
            {
                unsigned int prevCount = mCount - 1;
                // add to the things
                rotAnimDiffs.push_back(getRotDiff(branch[bCount] , master[mCount] , master[prevCount]));
                bCount++;
            }
        }
    }

    return rotAnimDiffs;
}

RotAnim DiffFunctions::getRotDiff(RotAnim rotA, RotAnim rotB)
{
    glm::quat mRot = rotA.rot;
    glm::quat bRot = rotB.rot;

    RotAnim rotDiff;

    rotDiff.rot = glm::inverse(mRot) * bRot;

    rotDiff.time = rotA.time;

    return rotDiff; 
}

RotAnim DiffFunctions::getRotDiff(RotAnim rotA, RotAnim rotB1, RotAnim rotB2)
{
    float interTime = (rotB1.time - rotB2.time) / (rotA.time - rotB2.time);

    glm::quat mRot = glm::slerp(rotB1.rot , rotB2.rot, interTime);
    glm::quat bRot = rotA.rot;

    RotAnim rotDiff;

    rotDiff.rot = glm::inverse(mRot) * bRot;

    rotDiff.time = rotA.time;

    return rotDiff; 
}
    
std::vector<ScaleAnim> DiffFunctions::getScaleDiffs(std::vector<ScaleAnim>& master, std::vector<ScaleAnim>& branch)
{
    std::vector<ScaleAnim> scaleAnimDiffs;

    // lets use count instead of iterators
    unsigned int masterSize = master.size();
    unsigned int branchSize = branch.size();

    unsigned int mCount = 0;
    unsigned int bCount = 0;

    while(mCount < masterSize || bCount < branchSize)
    {
        if(mCount == masterSize)
        {
            // just log branch because master is finished
            ScaleAnim scaleDiff;
            scaleDiff.time = branch[bCount].time;
            scaleDiff.scale = glm::vec3(0,0,0);
            scaleAnimDiffs.push_back(scaleDiff);
            bCount++;
        }
        else if(bCount == branchSize)
        {
            ScaleAnim scaleDiff;
            scaleDiff.time = master[mCount].time;
            scaleDiff.scale = glm::vec3(0,0,0);
            scaleAnimDiffs.push_back(scaleDiff);
            mCount++;
        }
        else
        {
            // we have a time match
            if(master[mCount].time == branch[bCount].time)
            {

                // add to the things
                scaleAnimDiffs.push_back(getScaleDiff(master[mCount] , branch[bCount]));
                mCount++;
                bCount++;
            }
            // check if one time is behind the other
            else if(master[mCount].time < branch[bCount].time)
            {
                // no matching times so make one 
                // calc interpolate amount
                auto prevCount = bCount - 1;
                scaleAnimDiffs.push_back(getScaleDiff(master[mCount] , branch[bCount] , branch[prevCount]));
                mCount++;
            }
            // now the branch is more than 
            else
            {
                unsigned int prevCount = mCount - 1;
                scaleAnimDiffs.push_back(getScaleDiff(branch[bCount] , master[mCount] , master[prevCount]));
                bCount++;
            }
        }
    }

    return scaleAnimDiffs;
}

ScaleAnim DiffFunctions::getScaleDiff(ScaleAnim scaleA, ScaleAnim scaleB)
{
    // we have a match lets compare the positions
    glm::vec3 scale1 = scaleA.scale;
    glm::vec3 scale2 = scaleB.scale;

    ScaleAnim scaleDiff;

    scaleDiff.scale.x = scale2.x - scale1.x;
    scaleDiff.scale.y = scale2.y - scale1.y;
    scaleDiff.scale.z = scale2.z - scale1.z;

    scaleDiff.time = scaleA.time;

    return scaleDiff; 
}

ScaleAnim DiffFunctions::getScaleDiff(ScaleAnim scaleA, ScaleAnim scaleB1, ScaleAnim scaleB2)
{
    float interTime = (scaleA.time - scaleB2.time) / (scaleB1.time - scaleB2.time);

    glm::vec3 interScale = glm::mix(scaleB1.scale , scaleB2.scale, interTime);
    glm::vec3 scale = scaleA.scale;

    ScaleAnim scaleDiff;

    scaleDiff.scale.x = scale.x - interScale.x;
    scaleDiff.scale.y = scale.y - interScale.y;
    scaleDiff.scale.z = scale.z - interScale.z;

    scaleDiff.time = scaleA.time;

    return scaleDiff; 
}

void DiffFunctions::getAnimMerge(DiffRig master, DiffRig branch, MergeRig &outRig)
{       
    // iterate through master bones
    for(auto bone : master.m_boneAnimDiffs)
    {
        // find matching one
        auto branchBone = branch.m_boneAnimDiffs.find(bone.first);

        if(branchBone != branch.m_boneAnimDiffs.end())
        {
            BoneAnimMerge boneDiff = getBoneMerge(bone.second, branchBone->second);
            outRig.m_boneAnimMerges.insert({bone.first, boneDiff});
        }
        else
        {
            std::cout << "cannot find matching bone for: " << bone.first << "\n";
        }
    }
}

BoneAnimMerge DiffFunctions::getBoneMerge(BoneAnimDiff master, BoneAnimDiff branch)
{
    BoneAnimMerge boneMerge;
    
    // now we check whether they both have changes on the same 
    bool bMasterChange = getIfChanges(master);
    bool bBranchChange = getIfChanges(branch);

    if(bMasterChange && bBranchChange)
    {
        boneMerge.m_type = CONFLICT;
        boneMerge.m_mainDiff = master;
        boneMerge.m_branchDiff = branch;
    }
    else if(bMasterChange)
    {
        boneMerge.m_type = MASTER;
        boneMerge.m_mainDiff = master;
    }
    else if(bBranchChange)
    {
        boneMerge.m_type = BRANCH;
        boneMerge.m_branchDiff = branch;
    }
    else
    {
        boneMerge.m_type = EMPTY;
    }
    
    return boneMerge;   
}

bool DiffFunctions::getIfChanges(BoneAnimDiff _diff)
{
    // loop all items if not zeros we got changes
    for(auto posDelta : _diff.m_posAnimDeltas)
    {
        glm::vec3 pos = posDelta.pos;

        if( pos != glm::vec3(0,0,0) )
        {
            return true;
        }
    }

    for(auto scaleDelta : _diff.m_scaleAnimDeltas)
    {
        glm::vec3 scale = scaleDelta.scale;

        if( scale != glm::vec3(0,0,0) )
        {
            return true;
        }
    }

    for(auto rotDelta : _diff.m_rotAnimDeltas)
    {
        glm::quat rot = rotDelta.rot;

        if( rot != glm::quat(0,0,0,0))
        {
            return true;
        }
    }

    return false;
}

