#include "include/mergedModel.h"

MergedModel::MergedModel()
{
    m_rig = std::shared_ptr<ModelRig>(new MergedRig);
}
