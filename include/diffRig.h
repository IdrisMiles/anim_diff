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

    void addBoneAnimDiff(std::string name, BoneAnimDiff boneDiff);

    double m_ticks;
    double m_duration;

private:

    std::unordered_map<std::string, BoneAnimDiff> m_boneAnimDiffs;

};

#endif // RIG_H
