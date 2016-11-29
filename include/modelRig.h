#ifndef MODELRIG_H
#define MODELRIG_H


#include <unordered_map>
#include "rig.h"
#include "boneAnim.h"

class ModelRig : public Rig
{
public:
    ModelRig();
    ~ModelRig();

    bool hasAnimation();

    double m_ticks;
    double m_duration;

    std::unordered_map<std::string, BoneAnim> m_boneAnims;

};

#endif // RIG_H
