#ifndef DIFFRIG_H
#define DIFFRIG_H

#include <vector>
#include <unordered_map>

#include "boneAnimDiff.h"
#include "rig.h"

class DiffRig : public Rig
{
public:
    DiffRig();
    ~DiffRig();

    void setTicks(double _ticks);
    void setDuration(double _duration);

    double getDuration();
    double getTicks();

    void addBoneAnimDiff(std::string name, BoneAnimDiff boneDiff);

private:

    double m_ticks;
    double m_duration;
    
    std::unordered_map<std::string, BoneAnimDiff> m_boneAnimDiffs;

};

#endif // RIG_H
