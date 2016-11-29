#ifndef REVISIONMERGE_H
#define REVISIONMERGE_H

#include "model.h"
#include "include/mergedRig.h"

class RevisionMerge
{
public:
    RevisionMerge();

    void LoadModel(const std::string &_modelFile);

    std::shared_ptr<Model> m_model;

//private:
    MergedRig m_rig;
};

#endif // REVISIONMERGE_H
