#include "diffRig.h"

DiffRig::DiffRig()
: Rig(),
m_ticks(0),
m_duration(0)
{

}

DiffRig::~DiffRig()
{
    
}

void DiffRig::addBoneAnimDiff(std::string name, BoneAnimDiff boneDiff)
{
    // sweet c++11 notation, check it out
    m_boneAnimDiffs.insert({name, boneDiff});
}