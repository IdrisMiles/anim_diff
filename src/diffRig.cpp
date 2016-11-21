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

void DiffRig::setTicks(double _ticks)
{
    m_ticks = _ticks;
}

void DiffRig::setDuration(double _duration)
{
    m_duration = _duration;
}

double DiffRig::getDuration()
{
    return m_duration;
}
    
double DiffRig::getTicks()
{
    return m_ticks;
}

void DiffRig::addBoneAnimDiff(std::string name, BoneAnimDiff boneDiff)
{
    // sweet c++11 notation, check it out
    m_boneAnimDiffs.insert({name, boneDiff});
}