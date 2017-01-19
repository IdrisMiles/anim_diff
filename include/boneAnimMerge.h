#ifndef BONEANIMMERGE_H
#define BONEANIMMERGE_H

#include <vector>
#include <string>

#include "boneAnimDiff.h"

class BoneAnimMerge
{
public:
    BoneAnimMerge();

    std::string m_name;

    MERGE m_type;
    
    BoneAnimDiff m_mainDiff;
    BoneAnimDiff m_branchDiff;
};

#endif // BONEANIMDIFF_H
