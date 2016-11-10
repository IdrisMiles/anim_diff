#include "include/revisionviewer.h"
#include <iostream>
#include <glm/ext.hpp>

glm::mat4 ConvertToGlmMat(const aiMatrix4x4 &m)
{
    glm::mat4 a(  m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);

    return a;
}

RevisionViewer::RevisionViewer(QWidget *parent) : OpenGLScene(parent)
{
    m_wireframe = false;
    m_drawMesh = true;
    m_playAnim = true;
    m_dt = 0.016;

    m_animTimer = new QTimer(this);
    connect(m_animTimer, SIGNAL(timeout()), this, SLOT(UpdateAnimation()));

    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(update()));
}


RevisionViewer::~RevisionViewer()
{
    m_meshVBO[SKINNED].destroy();
    m_meshNBO[SKINNED].destroy();
    m_meshIBO[SKINNED].destroy();
    m_meshBWBO[SKINNED].destroy();
    m_meshVAO[SKINNED].destroy();
    cleanup();
}

void RevisionViewer::LoadRevision(std::shared_ptr<RevisionNode> _revision)
{
    m_revision = _revision;
    m_scene = m_revision->m_model->m_scene;
    m_globalInverseTransform = m_scene->mRootNode->mTransformation;
    m_globalInverseTransform.Inverse();


    if(!m_scene)
    {
        std::cout<<"No valid AIScene\n";
    }
    else
    {
        InitMesh();
        InitRig();

    } // end if aiScene is valid

    InitVAO();

    m_animTimer->start(1000*m_dt);
    m_drawTimer->start(1000*m_dt);
}

void RevisionViewer::InitMesh()
{
    if(m_scene->HasMeshes())
    {
        unsigned int nb=0;
        unsigned int indexOffset = 0;
        for(unsigned int i=0; i<m_scene->mNumMeshes; i++)
        {
            // Mesh tris/element array
            unsigned int numFaces = m_scene->mMeshes[i]->mNumFaces;
            for(unsigned int f=0; f<numFaces; f++)
            {
                auto face = m_scene->mMeshes[i]->mFaces[f];
                m_meshTris.push_back(glm::ivec3(face.mIndices[0]+indexOffset, face.mIndices[1]+indexOffset, face.mIndices[2]+indexOffset));
            }

            // Mesh verts and norms
            unsigned int numVerts = m_scene->mMeshes[i]->mNumVertices;
            for(unsigned int v=0; v<numVerts; v++)
            {
                auto vert = m_scene->mMeshes[i]->mVertices[v];
                auto norm = m_scene->mMeshes[i]->mNormals[v];
                m_meshVerts.push_back(glm::vec3(vert.x, vert.y, vert.z));
                m_meshNorms.push_back(glm::vec3(norm.x, norm.y, norm.z));
            }


            m_meshBoneWeights.resize(m_meshVerts.size());

            // Mesh bones
            unsigned int numBones = m_scene->mMeshes[i]->mNumBones;
            for(unsigned int b=0; b<numBones; b++)
            {
                auto bone = m_scene->mMeshes[i]->mBones[b];
                unsigned int boneIndex = 0;
                std::string boneName = bone->mName.data;

                // Check this is a new bone
                if(m_boneMapping.find(boneName) == m_boneMapping.end())
                {
                    boneIndex = nb;
                    nb++;
                    m_boneInfo.push_back(BoneInfo());
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


    if(m_scene->HasAnimations())
    {
        m_animExists = true;
        m_ticksPerSecond = m_scene->mAnimations[0]->mTicksPerSecond;
        m_animationDuration = m_scene->mAnimations[0]->mDuration;
    }
    else
    {
        m_animExists = false;
        m_playAnim = false;

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

void RevisionViewer::InitRig()
{
    unsigned int id=0;
    aiMatrix4x4 mat;


    for(unsigned int i=0; i<m_scene->mRootNode->mNumChildren; i++)
    {
        GetRigTransforms(m_scene->mRootNode->mChildren[i], mat, id, 0);
    }
}

void RevisionViewer::GetRigTransforms(const aiNode* _pNode, const aiMatrix4x4 &_parentTransform, unsigned int &_currentID, const unsigned int _prevID)
{
    aiMatrix4x4 globalTransformation = _parentTransform * _pNode->mTransformation;

    unsigned int currentID = _prevID;

//    if (m_boneMapping.find(nodeName) != m_boneMapping.end()) {
//            uint BoneIndex = m_boneMapping[std::string(_pNode->mName.data)];
//            m_globalInverseTransform * _parentTransform * m_boneInfo[BoneIndex].boneOffset;
//        }

    if(FindNodeAnim(m_scene->mAnimations[0], _pNode->mName.data) != NULL)
    {
        m_rigVerts.push_back(glm::vec3(glm::vec4(0.0f,0.0f,0.0f,1.0f)*ConvertToGlmMat(globalTransformation)));
        m_rigNorms.push_back(glm::vec3(0.0f,0.0f,1.0f));

        VertexBoneData v2;
        std::string nodeName(_pNode->mName.data);
        v2.boneID[0] = m_boneMapping[nodeName];
        v2.boneWeight[0] = 1.0f;
        v2.boneWeight[1] = 0.0f;
        v2.boneWeight[2] = 0.0f;
        v2.boneWeight[3] = 0.0f;

        m_rigBoneWeights.push_back(v2);

        _currentID++;
        currentID = _currentID;
        m_rigElements.push_back((unsigned int)currentID);

    }
    else
    {
        std::cout<<"OOOOOOOO\n";
    }

    for (uint i = 0 ; i < _pNode->mNumChildren ; i++)
    {
        GetRigTransforms(_pNode->mChildren[i], globalTransformation, _currentID, currentID);
    }
}

void RevisionViewer::InitAnimation()
{

}

void RevisionViewer::InitVAO()
{

    m_shaderProg->bind();

    glPointSize(5);
    glLineWidth(10);

    // Get shader locations
    m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_colourLoc = m_shaderProg->uniformLocation("colour");
    glUniform3fv(m_colourLoc, 1, &m_colour[0]);
    m_vertAttrLoc = m_shaderProg->attributeLocation("vertex");
    m_normAttrLoc = m_shaderProg->attributeLocation("normal");
    m_boneIDAttrLoc = m_shaderProg->attributeLocation("BoneIDs");
    m_boneWeightAttrLoc = m_shaderProg->attributeLocation("Weights");
    m_boneUniformLoc = m_shaderProg->uniformLocation("Bones");

    std::cout<<"vert loc:\t"<<m_vertAttrLoc<<"\n";
    std::cout<<"norm loc:\t"<<m_normAttrLoc<<"\n";
    std::cout<<"boneID loc:\t"<<m_boneIDAttrLoc<<"\n";
    std::cout<<"boneWei loc:\t"<<m_boneWeightAttrLoc<<"\n";
    std::cout<<"Bones loc:\t"<<m_boneUniformLoc<<"\n";


    //--------------------------------------------------------------------------------------
    // Skinned mesh

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
    glEnableVertexAttribArray(m_vertAttrLoc);
    glVertexAttribPointer(m_vertAttrLoc, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO[SKINNED].release();


    // Setup our normals buffer object.
    m_meshNBO[SKINNED].create();
    m_meshNBO[SKINNED].bind();
    m_meshNBO[SKINNED].allocate(&m_meshNorms[0], m_meshNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_normAttrLoc);
    glVertexAttribPointer(m_normAttrLoc, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshNBO[SKINNED].release();


    // Set up vertex bone weighting buffer object
    m_meshBWBO[SKINNED].create();
    m_meshBWBO[SKINNED].bind();
    m_meshBWBO[SKINNED].allocate(&m_meshBoneWeights[0], m_meshBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_boneIDAttrLoc);
    glVertexAttribIPointer(m_boneIDAttrLoc, 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_boneWeightAttrLoc);
    glVertexAttribPointer(m_boneWeightAttrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)4);
    m_meshBWBO[SKINNED].release();

    m_meshVAO[SKINNED].release();


    //--------------------------------------------------------------------------------------
    // Rigged mesh

    m_meshVAO[RIG].create();
    m_meshVAO[RIG].bind();

    // Set up element array
    m_meshIBO[RIG].create();
    m_meshIBO[RIG].bind();
    m_meshIBO[RIG].allocate(&m_rigElements[0], m_rigElements.size() * sizeof(unsigned int));
    m_meshIBO[RIG].release();

    // Setup our vertex buffer object.
    m_meshVBO[RIG].create();
    m_meshVBO[RIG].bind();
    m_meshVBO[RIG].allocate(&m_rigVerts[0], m_rigVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_vertAttrLoc);
    glVertexAttribPointer(m_vertAttrLoc, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO[RIG].release();

    // Setup our normals buffer object.
    m_meshNBO[RIG].create();
    m_meshNBO[RIG].bind();
    m_meshNBO[RIG].allocate(&m_rigNorms[0], m_rigNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_normAttrLoc);
    glVertexAttribPointer(m_normAttrLoc, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshNBO[RIG].release();

    // Set up vertex bone weighting buffer object
    m_meshBWBO[RIG].create();
    m_meshBWBO[RIG].bind();
    m_meshBWBO[RIG].allocate(&m_rigBoneWeights[0], m_rigBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_boneIDAttrLoc);
    glVertexAttribIPointer(m_boneIDAttrLoc, 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_boneWeightAttrLoc);
    glVertexAttribPointer(m_boneWeightAttrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)4);
    m_meshBWBO[RIG].release();

    m_meshVAO[RIG].release();


    m_shaderProg->release();

}


void RevisionViewer::paintGL()
{

    // test hack
    static bool init = false;

    if(!init)
    {
        std::shared_ptr<RevisionNode> test(new RevisionNode());
        test->LoadModel("boblampclean.md5mesh");
        init = true;

        LoadRevision(test);
    }

    // clean gl window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // update model matrix
    m_modelMat = glm::mat4(1);
    m_modelMat = glm::translate(m_modelMat, glm::vec3(0,0, -0.1f*m_zDis));// m_zDis));
    m_modelMat = glm::rotate(m_modelMat, glm::radians(m_xRot/16.0f), glm::vec3(1,0,0));
    m_modelMat = glm::rotate(m_modelMat, glm::radians(m_yRot/16.0f), glm::vec3(0,1,0));


    // Set shader params
    m_shaderProg->bind();

    glUniformMatrix4fv(m_projMatrixLoc, 1, false, &m_projMat[0][0]);
    glUniformMatrix4fv(m_mvMatrixLoc, 1, false, &(m_modelMat*m_viewMat)[0][0]);
    glm::mat3 normalMatrix =  glm::inverse(glm::mat3(m_modelMat));
    glUniformMatrix3fv(m_normalMatrixLoc, 1, true, &normalMatrix[0][0]);
    glUniform3fv(m_colourLoc, 1, &m_colour[0]);


    //---------------------------------------------------------------------------------------
    // Draw code - replace this with project specific draw stuff
    //DrawMesh();
    DrawRig();
    //---------------------------------------------------------------------------------------


    m_shaderProg->release();

}

void RevisionViewer::DrawMesh()
{
    m_meshVAO[SKINNED].bind();
    glDrawElements(m_wireframe?GL_LINES:GL_TRIANGLES, 3*m_meshTris.size(), GL_UNSIGNED_INT, &m_meshTris[0]);
    m_meshVAO[SKINNED].release();
}

void RevisionViewer::DrawRig()
{
    m_meshVAO[RIG].bind();
    glDrawArrays(GL_POINTS, 0, m_rigVerts.size());
    //glDrawElements(GL_LINES, m_rigElements.size(), GL_UNSIGNED_INT, &m_rigElements[0]);
    m_meshVAO[RIG].release();
}

void RevisionViewer::UpdateAnimation()
{
    static float t = 0.0f;
    if(m_playAnim)
    {
        t += m_dt;
    }

    // Set shader params
    m_shaderProg->bind();
    UploadBonesToShader(t);
    m_shaderProg->release();
}

void RevisionViewer::UploadBonesToShader(const float _t)
{
    std::vector<glm::mat4> bones;
    BoneTransform(_t, bones);
    for(unsigned int b=0; b<bones.size() && b<100; b++)
    {
        glUniformMatrix4fv(m_boneUniformLoc + b, 1, true, &bones[b][0][0]);
    }
}


void RevisionViewer::BoneTransform(const float _t, std::vector<glm::mat4> &_transforms)
{
    aiMatrix4x4 identity = aiMatrix4x4();

    if(!m_animExists)
    {
        _transforms.resize(1);
        _transforms[0] = glm::mat4(1.0);
        return;
    }

    float timeInTicks = _t * m_ticksPerSecond;
    float animationTime = fmod(timeInTicks, m_animationDuration);
    std::cout<<"Anim time: "<<animationTime<<"\n";

    ReadNodeHierarchy(animationTime, m_scene->mAnimations[0], m_scene->mRootNode, identity);

    unsigned int numBones = m_boneInfo.size();
    _transforms.resize(numBones);

    for(unsigned int i=0; i<numBones; i++)
    {
        _transforms[i] = ConvertToGlmMat(m_boneInfo[i].finalTransform);
    }

}


void RevisionViewer::ReadNodeHierarchy(const float _animationTime, const aiAnimation* _pAnimation, const aiNode* _pNode, const aiMatrix4x4 &_parentTransform)
{
    if(!_pAnimation || !_pNode)
    {
        return;
    }

    std::string nodeName(_pNode->mName.data);

    aiMatrix4x4 nodeTransform(_pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(_pAnimation, nodeName);


    // Check if valid aiNodeAnim, as not all aiNode have corresponding aiNodeAnim.
    if (pNodeAnim)
    {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D scalingVec;
        CalcInterpolatedScaling(scalingVec, _animationTime, pNodeAnim);
        aiMatrix4x4 scalingMat = aiMatrix4x4();
        aiMatrix4x4::Scaling(scalingVec, scalingMat);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion rotationQ = aiQuaternion();
        CalcInterpolatedRotation(rotationQ, _animationTime, pNodeAnim);
        aiMatrix4x4 rotationMat = aiMatrix4x4();
        rotationMat = aiMatrix4x4(rotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D translationVec;
        CalcInterpolatedPosition(translationVec, _animationTime, pNodeAnim);
        aiMatrix4x4 translationMat = aiMatrix4x4();
        aiMatrix4x4::Translation(translationVec, translationMat);

        // Combine the above transformations
        nodeTransform = translationMat * rotationMat * scalingMat;
    }

    aiMatrix4x4 globalTransformation = _parentTransform * nodeTransform;

    if (m_boneMapping.find(nodeName) != m_boneMapping.end()) {
        uint BoneIndex = m_boneMapping[nodeName];
        m_boneInfo[BoneIndex].finalTransform = m_globalInverseTransform * globalTransformation * m_boneInfo[BoneIndex].boneOffset;
    }

    for (uint i = 0 ; i < _pNode->mNumChildren ; i++) {
        ReadNodeHierarchy(_animationTime, _pAnimation, _pNode->mChildren[i], globalTransformation);
    }

}


const aiNodeAnim* RevisionViewer::FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName)
{
    for (uint i = 0 ; i < _pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = _pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == _nodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

void RevisionViewer::CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    // we need at least two values to interpolate...
    if (_pNodeAnim->mNumRotationKeys == 1) {
        _out = _pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotationKeyFrame(_animationTime, _pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < _pNodeAnim->mNumRotationKeys);
    float DeltaTime = _pNodeAnim->mRotationKeys[NextRotationIndex].mTime - _pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (_animationTime - (float)_pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = _pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = _pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(_out, StartRotationQ, EndRotationQ, Factor);
    _out = _out.Normalize();
}

void RevisionViewer::CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    // we need at least two values to interpolate...
    if (_pNodeAnim->mNumPositionKeys == 1) {
        _out = _pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPositionKeyFrame(_animationTime, _pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < _pNodeAnim->mNumPositionKeys);
    float DeltaTime = _pNodeAnim->mPositionKeys[NextPositionIndex].mTime - _pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float Factor = (_animationTime - (float)_pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& startPositionV = _pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& endPositionV = _pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    _out = startPositionV + (Factor*(endPositionV-startPositionV));

}

void RevisionViewer::CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    // we need at least two values to interpolate...
    if (_pNodeAnim->mNumScalingKeys == 1) {
        _out = _pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint scalingIndex = FindScalingKeyFrame(_animationTime, _pNodeAnim);
    uint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < _pNodeAnim->mNumScalingKeys);
    float DeltaTime = _pNodeAnim->mScalingKeys[nextScalingIndex].mTime - _pNodeAnim->mScalingKeys[scalingIndex].mTime;
    float Factor = (_animationTime - (float)_pNodeAnim->mScalingKeys[scalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& startScalingV = _pNodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& endScalingV = _pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
    _out = startScalingV + (Factor*(endScalingV-startScalingV));

}

uint RevisionViewer::FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    assert(_pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < _pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (_animationTime < (float)_pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}

uint RevisionViewer::FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    assert(_pNodeAnim->mNumPositionKeys > 0);

    for (uint i = 0 ; i < _pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (_animationTime < (float)_pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}

uint RevisionViewer::FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    assert(_pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0 ; i < _pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (_animationTime < (float)_pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}
