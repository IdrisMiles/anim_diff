#include "include/model.h"
#include <iostream>

Model::Model()
{

}

Model::~Model()
{

}


void Model::LoadModel(const std::string &_modelFile)
{
    // Load mesh with ASSIMP
    Assimp::Importer importer;
    m_scene = importer.ReadFile(_modelFile,
                                    aiProcess_GenSmoothNormals |
                                    aiProcess_Triangulate |
                                    aiProcess_JoinIdenticalVertices |
                                    aiProcess_SortByPType);
    if(!m_scene)
    {
        std::cout<<"Error loading "<<_modelFile<<" with assimp\n";
    }
}
