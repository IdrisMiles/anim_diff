#ifndef MERGEDRIG_H
#define MERGEDRIG_H

#include "include/modelRig.h"

class MergedRig : ModelRig
{
public:
    MergedRig();
    virtual ~MergedRig();

private:
    std::vector<std::unordered_map<std::string, int>> m_boneDiffFlag;
};

#endif // MERGEDRIG_H
