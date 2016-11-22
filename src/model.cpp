#include "model.h"
#include <iostream>

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

    m_globalInverseTransform = ViewerUtilities::ConvertToGlmMat(scene->mRootNode->mTransformation);
    m_globalInverseTransform = glm::inverse(m_globalInverseTransform);

    if(!scene)
    {
        std::cout<<"No valid AIScene\n";
    }
    else
    {
        InitMesh(scene);
        InitRig(scene);


        for(int i=0; i<scene->mNumMeshes; i++)
        {
            for(int j=0; j<scene->mMeshes[i]->mNumBones; j++)
            {
                std::cout<<std::string(scene->mMeshes[i]->mBones[j]->mName.data)<<"\n";
            }
        }
        std::cout<<"_______________________________\nBones:\n";
        for(auto a : m_boneMapping)
        {
            std::cout<<"["<<a.first<<"] : "<< a.second<<"\n";
        }
        std::cout<<"_______________________________\n";

        m_rig.m_rootBone = std::shared_ptr<Bone>(new Bone());
        m_rig.m_rootBone->m_name = std::string(scene->mRootNode->mName.data);
        m_rig.m_rootBone->m_transform = ViewerUtilities::ConvertToGlmMat(scene->mRootNode->mTransformation);
        m_rig.m_rootBone->m_boneOffset = glm::mat4(1.0f);
        m_rig.m_rootBone->m_parent = NULL;


        unsigned int numChildren = scene->mRootNode->mNumChildren;
        for (unsigned int i=0; i<numChildren; i++)
        {
            CopyRigStructure(scene, scene->mRootNode->mChildren[i], m_rig.m_rootBone, ViewerUtilities::ConvertToGlmMat(scene->mRootNode->mTransformation));
        }
    }

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
    glm::mat4 mat = ViewerUtilities::ConvertToGlmMat(_scene->mRootNode->mTransformation) * m_globalInverseTransform;

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


void Model::SetRigVerts(aiNode* _pParentNode, aiNode* _pNode, const glm::mat4 &_parentTransform, const glm::mat4 &_thisTransform)
{
    const std::string parentNodeName(_pParentNode->mName.data);
    const std::string nodeName = _pNode->mName.data;
    bool isBone = m_boneMapping.find(nodeName) != m_boneMapping.end();

    glm::mat4 newThisTransform = ViewerUtilities::ConvertToGlmMat(_pNode->mTransformation) * _thisTransform;
    glm::mat4 newParentTransform = _parentTransform;
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

void Model::SetJointVert(const std::string _nodeName, const glm::mat4 &_transform, VertexBoneData &_vb)
{
    if(m_boneMapping.find(_nodeName) != m_boneMapping.end())
    {
        _vb.boneID[0] = m_boneMapping[_nodeName];
        _vb.boneWeight[0] = 1.0f;
        _vb.boneWeight[1] = 0.0f;
        _vb.boneWeight[2] = 0.0f;
        _vb.boneWeight[3] = 0.0f;

        m_rigVerts.push_back(glm::vec3(glm::vec4(0.0f,0.0f,0.0f,1.0f) * _transform));
        m_rigJointColours.push_back(glm::vec3(0.4f, 1.0f, 0.4f));
        m_rigBoneWeights.push_back(_vb);
    }
    else
    {
        std::cout<<"This Node is not a bone, skipping\n";
    }

}

void Model::CopyRigStructure(const aiScene *_aiScene, aiNode *_aiNode, std::shared_ptr<Bone> _parentBone, const glm::mat4 &_parentTransform)
{
    std::cout<<"\n";
    std::shared_ptr<Bone> newBone = std::shared_ptr<Bone>(new Bone());
    glm::mat4 newParentTransform = _parentTransform;

    // Check if this is a bone
    if(m_boneMapping.find(std::string(_aiNode->mName.data)) != m_boneMapping.end())
    {
        newBone->m_name = std::string(_aiNode->mName.data);
        newBone->m_transform = ViewerUtilities::ConvertToGlmMat(_aiNode->mTransformation) * _parentTransform;

        // Get bone offset matrix
        const aiBone* paiBone = ViewerUtilities::GetBone(_aiScene, newBone->m_name);
        if(paiBone)
        {
            std::cout<<std::string(paiBone->mName.data)<< "valid bone\n";
            newBone->m_boneOffset = ViewerUtilities::ConvertToGlmMat(paiBone->mOffsetMatrix);
        }
        else
        {
            std::cout<<"Well sheet, didn't find a bone in aiScene with name matching: "<<newBone->m_name<<". Thus not boneOffsetMatrix\n";
            newBone->m_boneOffset = glm::mat4(1.0f);
        }

        // Get animation data
        const aiNodeAnim *pNodeAnim = ViewerUtilities::FindNodeAnim(_aiScene->mAnimations[_aiScene->mNumAnimations-1], newBone->m_name);
        if(pNodeAnim)
        {
            std::cout<<std::string(pNodeAnim->mNodeName.data)<<" valid nodeAnim\n";
            m_rig.m_boneAnims[newBone->m_name] = TransferAnim(pNodeAnim);
            newBone->m_boneAnim = &m_rig.m_boneAnims[newBone->m_name];

        }
        else
        {
            std::cout<<"Daaannnng, didn't find aiNodeAnim in aiAnimation["<<_aiScene->mNumAnimations<<"] with matching name: "<<newBone->m_name<<". Thus No animation.\n";
        }

        // Set parent and set child
        newBone->m_parent = _parentBone;
        _parentBone->m_children.push_back(newBone);

        newParentTransform = glm::mat4(1.0f);
    }
    else
    {
        // forward on this nodes transform to affect the next bone
        std::cout<<"Model::CopyRigStructure | "<<std::string(_aiNode->mName.data)<<" Is not a Bone, probably an arbitrary transform.\n";
        newBone = _parentBone;
        newParentTransform = ViewerUtilities::ConvertToGlmMat(_aiNode->mTransformation) * _parentTransform;
    }


    unsigned int numChildren = _aiNode->mNumChildren;
    for (unsigned int i=0; i<numChildren; i++)
    {
        CopyRigStructure(_aiScene, _aiNode->mChildren[i], newBone, newParentTransform);
    }
}


BoneAnim Model::TransferAnim(const aiNodeAnim *_pNodeAnim)
{
    BoneAnim newBoneAnim;

    if(_pNodeAnim != NULL)
    {
        // Rotation animation
        for(unsigned int i=0; i<_pNodeAnim->mNumRotationKeys; i++)
        {
            float time = _pNodeAnim->mRotationKeys[i].mTime;
            glm::quat rot;
            rot.x = _pNodeAnim->mRotationKeys[i].mValue.x;
            rot.y = _pNodeAnim->mRotationKeys[i].mValue.y;
            rot.z = _pNodeAnim->mRotationKeys[i].mValue.z;
            rot.w = _pNodeAnim->mRotationKeys[i].mValue.w;
            RotAnim rotAnim = {time, rot};
            newBoneAnim.m_rotAnim.push_back(rotAnim);
        }

        // Position animation
        for(unsigned int i=0; i<_pNodeAnim->mNumPositionKeys; i++)
        {
            float time = _pNodeAnim->mPositionKeys[i].mTime;
            glm::vec3 pos;
            pos.x = _pNodeAnim->mPositionKeys[i].mValue.x;
            pos.y = _pNodeAnim->mPositionKeys[i].mValue.y;
            pos.z = _pNodeAnim->mPositionKeys[i].mValue.z;
            PosAnim posAnim = {time, pos};
            newBoneAnim.m_posAnim.push_back(posAnim);
        }

        // Scaling animation
        for(unsigned int i=0; i<_pNodeAnim->mNumScalingKeys; i++)
        {
            float time = _pNodeAnim->mScalingKeys[i].mTime;
            glm::vec3 scale;
            scale.x = _pNodeAnim->mScalingKeys[i].mValue.x;
            scale.y = _pNodeAnim->mScalingKeys[i].mValue.y;
            scale.z = _pNodeAnim->mScalingKeys[i].mValue.z;
            ScaleAnim scaleAnim = {time, scale};
            newBoneAnim.m_scaleAnim.push_back(scaleAnim);
        }
    }
    else
    {
        std::cout<<"Invalid aiNodeAnim\n";
    }

    return newBoneAnim;
}
