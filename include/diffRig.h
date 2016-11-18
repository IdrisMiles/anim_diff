#ifndef DIFFRIG_H
#define DIFFRIG_H

#include <vector>
#include <unordered_map>

#include "boneAnimDiff.h"

class DiffRig
{
public:
    DiffRig();
    ~DiffRig(){};

private:
    
    std::vector<std::unordered_map<std::string, BoneAnimDiff>> m_boneAnimDiffs

};

#endif // RIG_H
