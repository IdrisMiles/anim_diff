#ifndef REVISIONNODE_H
#define REVISIONNODE_H

#include "model.h"

class RevisionNode
{
public:
    RevisionNode();
    ~RevisionNode();


    void LoadModel(const std::string &_modelFile);

    std::shared_ptr<Model> m_model;
};

#endif // REVISIONNODE_H
