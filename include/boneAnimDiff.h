#ifndef BONEANIMDIFF_H
#define BONEANIMDIFF_H

#include <vector>
#include <string>

#include "boneAnim.h"

class BoneAnimDiff
{
public:
    BoneAnimDiff();

    std::string m_name;
    std::vector<PosAnim> m_posAnimDeltas;
    std::vector<ScaleAnim> m_scaleAnimDeltas;
    std::vector<RotAnim> m_rotAnimDeltas;
};

#endif // BONEANIMDIFF_H
