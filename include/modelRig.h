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

private:
    
    std::vector<std::unordered_map<std::string, BoneAnim>> m_boneAnims;

};

#endif // RIG_H
