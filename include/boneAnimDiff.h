#ifndef BONEANIMDIFF_H
#define BONEANIMDIFF_H

#include <vector>
#include <string>

#include "include/boneAnimDelta.h"

class BoneAnimDiff
{
public:
    BoneAnimDiff();

    std::string m_name;
    std::vector<BoneAnimDelta> m_boneAnimDelta;
};

#endif // BONEANIMDIFF_H
