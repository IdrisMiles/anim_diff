#ifndef MERGEDRIG_H
#define MERGEDRIG_H

#include "modelRig.h"

enum DiffFlagEnum {NONE = 0, POS = 1 << 0, ROT = 1 << 1, SCALE = 1 << 2};

struct DiffFlagAnim
{
    float time;
    DiffFlagEnum flag;
};

class MergedRig : public ModelRig
{
public:
    MergedRig();
    virtual ~MergedRig();

    std::unordered_map<std::string, std::vector<DiffFlagAnim>> m_boneDiffFlag;
};

#endif // MERGEDRIG_H
