#ifndef MODEL_H
#define MODEL_H

#include "modelRig.h"

class Model
{
public:
    Model();
    ~Model();

    void LoadModel(const std::string &_modelFile);

    const ModelRig getRig();

private:
    ModelRig m_rig;
};

#endif // MODEL_H
