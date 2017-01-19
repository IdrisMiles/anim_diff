#ifndef MERGERIG_H
#define MERGERIG_H

#include <vector>
#include <unordered_map>

#include "boneAnimMerge.h"
#include "rig.h"

class MergeRig : public Rig
{
public:
    MergeRig();
    ~MergeRig();

    double m_ticks;
    double m_duration;

    std::unordered_map<std::string, BoneAnimMerge> m_boneAnimMerges;
};

#endif // RIG_H