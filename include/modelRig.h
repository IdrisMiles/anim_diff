#ifndef MODELRIG_H
#define MODELRIG_H

#include <vector>
#include <unordered_map>

#include "boneAnim.h"
#include "rig.h"

class ModelRig : public Rig
{
public:
    ModelRig();
    ~ModelRig();

    double getTicks();
    double getDuration();
    bool hasAnimation();

private:

    double m_ticks;
    double m_duration;

    std::vector<std::unordered_map<std::string, BoneAnim>> m_boneAnims;

};

#endif // RIG_H
