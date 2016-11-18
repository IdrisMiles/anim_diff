#ifndef RIG_H
#define RIG_H

#include <vector>

#include "bone.h"
#include "boneAnim.h"
#include "boneAnimDiff.h"


class Rig
{
public:
    Rig();


private:
    Bone* m_rootBone;

    // vector of vectors because we can have multiple animations
    std::vector<std::vector<BoneAnim>> m_boneAnims;
    std::vector<std::vector<BoneAnimDiff>> m_boneAnimDiff;

};

#endif // RIG_H
