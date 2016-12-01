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

    double m_ticks;
    double m_duration;

    std::unordered_map<std::string, BoneAnimDiff> m_boneAnimDiffs;
};

#endif // RIG_H
