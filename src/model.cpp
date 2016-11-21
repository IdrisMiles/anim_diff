#include "model.h"
#include <iostream>

#include <QOpenGLShaderProgram>

Model::Model()
{

}

Model::~Model()
{

}


void Model::LoadModel(const std::string &_modelFile)
{
    const aiScene *scene;
    Assimp::Importer m_importer;

    // Load mesh with ASSIMP
    scene = m_importer.ReadFile(_modelFile,
                                    aiProcess_GenSmoothNormals |
                                    aiProcess_Triangulate |
                                    aiProcess_JoinIdenticalVertices |
                                    aiProcess_SortByPType);
    if(!scene)
    {
        std::cout<<"Error loading "<<_modelFile<<" with assimp\n";
    }

//    m_revision = _revision;
//    _scene = m_revision->m_model->scene;
    m_globalInverseTransform = scene->mRootNode->mTransformation;
    m_globalInverseTransform.Inverse();

    if(!scene)
    {
        std::cout<<"No valid AIScene\n";
    }
    else
    {
        InitMesh(scene);
        InitRig(scene);

    } // end if aiScene is valid

}


void Model::SetShader(QOpenGLShaderProgram **_shaderProg)
{
    m_shaderProg[SKINNED] = _shaderProg[SKINNED];
    m_shaderProg[RIG] = _shaderProg[RIG];
}

void Model::InitMesh(const aiScene *_scene)
{
    if(_scene->HasMeshes())
    {
        unsigned int nb=0;
        unsigned int indexOffset = 0;
        for(unsigned int i=0; i<_scene->mNumMeshes; i++)
        {
            // Mesh tris/element array
            unsigned int numFaces = _scene->mMeshes[i]->mNumFaces;
            for(unsigned int f=0; f<numFaces; f++)
            {
                auto face = _scene->mMeshes[i]->mFaces[f];
                m_meshTris.push_back(glm::ivec3(face.mIndices[0]+indexOffset, face.mIndices[1]+indexOffset, face.mIndices[2]+indexOffset));
            }

            // Mesh verts and norms
            unsigned int numVerts = _scene->mMeshes[i]->mNumVertices;
            for(unsigned int v=0; v<numVerts; v++)
            {
                auto vert = _scene->mMeshes[i]->mVertices[v];
                auto norm = _scene->mMeshes[i]->mNormals[v];
                m_meshVerts.push_back(glm::vec3(vert.x, vert.y, vert.z));
                m_meshNorms.push_back(glm::vec3(norm.x, norm.y, norm.z));
            }


            m_meshBoneWeights.resize(m_meshVerts.size());

            // Mesh bones
            unsigned int numBones = _scene->mMeshes[i]->mNumBones;
            for(unsigned int b=0; b<numBones; b++)
            {
                auto bone = _scene->mMeshes[i]->mBones[b];
                unsigned int boneIndex = 0;
                std::string boneName = bone->mName.data;

                // Check this is a new bone
                if(m_boneMapping.find(boneName) == m_boneMapping.end())
                {
                    boneIndex = nb;
                    nb++;
                    m_boneInfo.push_back(BoneTransformData());
                    m_boneMapping[boneName] = boneIndex;
                    m_boneInfo[boneIndex].boneOffset = bone->mOffsetMatrix;
                }
                else
                {
                    boneIndex = m_boneMapping[boneName];

                }


                // Bone vertex weights
                unsigned int boneWeights = bone->mNumWeights;
                for(unsigned int bw=0; bw<boneWeights; bw++)
                {
                    unsigned int vertexID = indexOffset + bone->mWeights[bw].mVertexId;
                    float vertexWeight = bone->mWeights[bw].mWeight;
                    for(unsigned int w=0; w<4; w++)
                    {
                        if(m_meshBoneWeights[vertexID].boneWeight[w] == 0.0f)
                        {
                            m_meshBoneWeights[vertexID].boneWeight[w] = vertexWeight;
                            m_meshBoneWeights[vertexID].boneID[w] = boneIndex;
                        }
                    }
                }

            } // end for numBones

            indexOffset = m_meshVerts.size();

        } // end for numMeshes

    }// end if has mesh


    if(_scene->HasAnimations())
    {
        m_animExists = true;
        m_numAnimations = _scene->mNumAnimations;
        m_animationID = m_numAnimations - 1;
        m_ticksPerSecond = _scene->mAnimations[m_animationID]->mTicksPerSecond;
        m_animationDuration = _scene->mAnimations[m_animationID]->mDuration;

        std::cout<<"Animation duration:\t"<<m_animationDuration<<"\n";
        std::cout<<"Number of animations:\t"<<_scene->mNumAnimations<<"\n";
    }
    else
    {
        m_animExists = false;

        for(unsigned int bw=0; bw<m_meshVerts.size();bw++)
        {
            m_meshBoneWeights[bw].boneID[0] = 0;
            m_meshBoneWeights[bw].boneID[1] = 0;
            m_meshBoneWeights[bw].boneID[2] = 0;
            m_meshBoneWeights[bw].boneID[3] = 0;

            m_meshBoneWeights[bw].boneWeight[0] = 1.0;
            m_meshBoneWeights[bw].boneWeight[1] = 0.0;
            m_meshBoneWeights[bw].boneWeight[2] = 0.0;
            m_meshBoneWeights[bw].boneWeight[3] = 0.0;
        }
    }
}

void Model::InitRig(const aiScene *_scene)
{
    aiMatrix4x4 mat = _scene->mRootNode->mTransformation * m_globalInverseTransform;

    for (uint i = 0 ; i < _scene->mRootNode->mNumChildren ; i++)
    {
        SetRigVerts(_scene->mRootNode, _scene->mRootNode->mChildren[i], mat, mat);
    }

    if(m_rigVerts.size() % 2)
    {
        for(unsigned int i=0; i<m_rigVerts.size()/2; i++)
        {
            int id = i*2;
            if(m_rigVerts[id] == m_rigVerts[id+1])
            {
                std::cout<<"Repeated joint causing rig issue, removing joint\n";
                m_rigVerts.erase(m_rigVerts.begin()+id);
                m_rigJointColours.erase(m_rigJointColours.begin()+id);
                m_rigBoneWeights.erase(m_rigBoneWeights.begin()+id);

                break;
            }
        }
    }

    std::cout<<"Number of rig verts:\t"<<m_rigVerts.size()<<"\n";

}


void Model::SetRigVerts(aiNode* _pParentNode, aiNode* _pNode, const aiMatrix4x4 &_parentTransform, const aiMatrix4x4 &_thisTransform)
{
    const std::string parentNodeName(_pParentNode->mName.data);
    const std::string nodeName = _pNode->mName.data;
    bool isBone = m_boneMapping.find(nodeName) != m_boneMapping.end();

    aiMatrix4x4 newThisTransform = _thisTransform * _pNode->mTransformation;
    aiMatrix4x4 newParentTransform = _parentTransform;
    aiNode* newParent = _pParentNode;

    VertexBoneData v2;

    if(isBone)
    {
        // parent joint
        SetJointVert(parentNodeName, _parentTransform, v2);

        // This joint
        SetJointVert(nodeName, newThisTransform, v2);

        // This joint becomes new parent
        newParentTransform = newThisTransform;
        newParent = _pNode;
    }


    // Repeat for rest of the joints
    for (uint i = 0 ; i < _pNode->mNumChildren ; i++)
    {
        SetRigVerts(newParent, _pNode->mChildren[i], newParentTransform, newThisTransform);
    }
}

void Model::SetJointVert(const std::string _nodeName, const aiMatrix4x4 &_transform, VertexBoneData &_vb)
{
    if(m_boneMapping.find(_nodeName) != m_boneMapping.end())
    {
        _vb.boneID[0] = m_boneMapping[_nodeName];
        _vb.boneWeight[0] = 1.0f;
        _vb.boneWeight[1] = 0.0f;
        _vb.boneWeight[2] = 0.0f;
        _vb.boneWeight[3] = 0.0f;

        m_rigVerts.push_back(glm::vec3(glm::vec4(0.0f,0.0f,0.0f,1.0f)*ViewerUtilities::ConvertToGlmMat(_transform)));
        m_rigJointColours.push_back(glm::vec3(0.4f, 1.0f, 0.4f));
        m_rigBoneWeights.push_back(_vb);
    }
    else
    {
        std::cout<<"This Node is not a bone, skipping\n";
    }

}

/*
void Model::InitVAO()
{

    glPointSize(10);

    //--------------------------------------------------------------------------------------
    // Skinned mesh
    m_shaderProg[SKINNED]->bind();

    // Get shader locations
    m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_colourLoc[SKINNED] = m_shaderProg[SKINNED]->uniformLocation("uColour");
    glUniform3fv(m_colourLoc[SKINNED], 1, &m_colour[0]);
    m_vertAttrLoc[SKINNED] = m_shaderProg[SKINNED]->attributeLocation("vertex");
    m_normAttrLoc[SKINNED] = m_shaderProg[SKINNED]->attributeLocation("normal");
    m_boneIDAttrLoc[SKINNED] = m_shaderProg[SKINNED]->attributeLocation("BoneIDs");
    m_boneWeightAttrLoc[SKINNED] = m_shaderProg[SKINNED]->attributeLocation("Weights");
    m_boneUniformLoc[SKINNED] = m_shaderProg[SKINNED]->uniformLocation("Bones");

    std::cout<<"SKINNED | vert Attr loc:\t"<<m_vertAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | norm Attr loc:\t"<<m_normAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | boneID Attr loc:\t"<<m_boneIDAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | boneWei Attr loc:\t"<<m_boneWeightAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | Bones Unif loc:\t"<<m_boneUniformLoc[SKINNED]<<"\n";


    // Set up VAO
    m_meshVAO[SKINNED].create();
    m_meshVAO[SKINNED].bind();

    // Set up element array
    m_meshIBO[SKINNED].create();
    m_meshIBO[SKINNED].bind();
    m_meshIBO[SKINNED].allocate(&m_meshTris[0], m_meshTris.size() * sizeof(int));
    m_meshIBO[SKINNED].release();


    // Setup our vertex buffer object.
    m_meshVBO[SKINNED].create();
    m_meshVBO[SKINNED].bind();
    m_meshVBO[SKINNED].allocate(&m_meshVerts[0], m_meshVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_vertAttrLoc[SKINNED]);
    glVertexAttribPointer(m_vertAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO[SKINNED].release();


    // Setup our normals buffer object.
    m_meshNBO[SKINNED].create();
    m_meshNBO[SKINNED].bind();
    m_meshNBO[SKINNED].allocate(&m_meshNorms[0], m_meshNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_normAttrLoc[SKINNED]);
    glVertexAttribPointer(m_normAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshNBO[SKINNED].release();


    // Set up vertex bone weighting buffer object
    m_meshBWBO[SKINNED].create();
    m_meshBWBO[SKINNED].bind();
    m_meshBWBO[SKINNED].allocate(&m_meshBoneWeights[0], m_meshBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_boneIDAttrLoc[SKINNED]);
    glVertexAttribIPointer(m_boneIDAttrLoc[SKINNED], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_boneWeightAttrLoc[SKINNED]);
    glVertexAttribPointer(m_boneWeightAttrLoc[SKINNED], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(4*sizeof(unsigned int)));
    m_meshBWBO[SKINNED].release();

    m_meshVAO[SKINNED].release();
    m_shaderProg[SKINNED]->release();



    //--------------------------------------------------------------------------------------
    // Rigged mesh
    m_shaderProg[RIG]->bind();

    // Get shader locations
    m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_vertAttrLoc[RIG] = m_shaderProg[RIG]->attributeLocation("vertex");
    m_boneIDAttrLoc[RIG] = m_shaderProg[RIG]->attributeLocation("BoneIDs");
    m_boneWeightAttrLoc[RIG] = m_shaderProg[RIG]->attributeLocation("Weights");
    m_boneUniformLoc[RIG] = m_shaderProg[RIG]->uniformLocation("Bones");
    m_colourAttrLoc[RIG] = m_shaderProg[RIG]->attributeLocation("colour");


    std::cout<<"RIG | vert Attr loc:\t"<<m_vertAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | boneID Attr loc:\t"<<m_boneIDAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | boneWei Attr loc:\t"<<m_boneWeightAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | Bones Unif loc:\t"<<m_boneUniformLoc[RIG]<<"\n";
    std::cout<<"RIG | colour Attr loc: "<<m_colourAttrLoc[RIG]<<"\n";


    m_meshVAO[RIG].create();
    m_meshVAO[RIG].bind();

    // Setup our vertex buffer object.
    m_meshVBO[RIG].create();
    m_meshVBO[RIG].bind();
    m_meshVBO[RIG].allocate(&m_rigVerts[0], m_rigVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_vertAttrLoc[RIG]);
    glVertexAttribPointer(m_vertAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO[RIG].release();

    // Set up our Rig joint colour buffer object
    m_meshCBO[RIG].create();
    m_meshCBO[RIG].bind();
    m_meshCBO[RIG].allocate(&m_rigJointColours[0], m_rigJointColours.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_colourAttrLoc[RIG]);
    glVertexAttribPointer(m_colourAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshCBO[RIG].release();

    // Set up vertex bone weighting buffer object
    m_meshBWBO[RIG].create();
    m_meshBWBO[RIG].bind();
    m_meshBWBO[RIG].allocate(&m_rigBoneWeights[0], m_rigBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_boneIDAttrLoc[RIG]);
    glVertexAttribIPointer(m_boneIDAttrLoc[RIG], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_boneWeightAttrLoc[RIG]);
    glVertexAttribPointer(m_boneWeightAttrLoc[RIG], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*) (4*sizeof(unsigned int)));
    m_meshBWBO[RIG].release();

    m_meshVAO[RIG].release();

    m_shaderProg[RIG]->release();

}
*/
