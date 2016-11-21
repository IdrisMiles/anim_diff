#include "model.h"
#include <iostream>

// ASSIMP includes
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

Model::Model()
{

}

Model::~Model()
{

}


void Model::LoadModel(const std::string &_modelFile)
{
    // Load mesh with ASSIMP
    const aiScene *scene = aiImportFile(_modelFile.c_str(),
                                    aiProcess_GenSmoothNormals |
                                    aiProcess_Triangulate |
                                    aiProcess_JoinIdenticalVertices |
                                    aiProcess_SortByPType);
    if(!scene)
    {
        std::cout<<"Error loading "<<_modelFile<<" with assimp\n";
    }
}

const ModelRig Model::getRig()
{
    return m_rig;
}
