#ifndef MODEL_H
#define MODEL_H

// ASSIMP includes
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <memory>

class Model
{
public:
    Model();
    ~Model();

    void LoadModel(const std::string &_modelFile);

    const aiScene *m_scene;
};

#endif // MODEL_H
