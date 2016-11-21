#include "modelRig.h"

ModelRig::ModelRig()
: Rig(),
m_ticks(0),
m_duration(0)
{

}

ModelRig::~ModelRig()
{
    
}

bool ModelRig::hasAnimation()
{
    return m_boneAnims.empty();
}

double ModelRig::getTicks()
{
    return m_ticks;
}

double ModelRig::getDuration()
{
    return m_duration;
}