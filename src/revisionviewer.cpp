#include "include/revisionviewer.h"
#include <iostream>


RevisionViewer::RevisionViewer(QWidget *parent) : OpenGLScene(parent)
{
    m_wireframe = false;
    m_drawMesh = true;
}


RevisionViewer::~RevisionViewer()
{
    cleanup();
}


void RevisionViewer::paintGL()
{

    // test hack
    static bool init = false;

    if(!init)
    {
        std::shared_ptr<RevisionNode> test(new RevisionNode());
        test->LoadModel("pighead.obj");
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
    m_meshVAO.bind();
    glDrawElements(m_wireframe?GL_LINES:GL_TRIANGLES, 3*m_meshTris.size(), GL_UNSIGNED_INT, &m_meshTris[0]);
    m_meshVAO.release();

    //---------------------------------------------------------------------------------------


    m_shaderProg->release();
}

void RevisionViewer::LoadRevision(std::shared_ptr<RevisionNode> _revision)
{

    m_scene = _revision->m_model->m_scene;
    m_globalInverseTransform = m_scene->mRootNode->mTransformation;
    m_globalInverseTransform.Inverse();


    if(!m_scene)
    {
        std::cout<<"No valid AIScene\n";
    }
    else
    {
        if(m_scene->HasMeshes())
        {
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

                // Mesh bones
                unsigned int nb=0;
                unsigned int numBones = m_scene->mMeshes[i]->mNumBones;
                for(unsigned int b=0; b<numBones; b++)
                {
                    auto bone = m_scene->mMeshes[i]->mBones[b];
                    unsigned int boneIndex = 0;
                    std::string boneName = bone->mName.data;

                    if(m_boneMapping.find(boneName) == m_boneMapping.end())
                    {
                        boneIndex = nb;
                        nb++;
                        m_boneInfo.push_back(BoneInfo());
                    }
                    else
                    {
                        boneIndex = m_boneMapping[boneName];

                    }

                    m_boneMapping[boneName] = boneIndex;
                    m_boneInfo[boneIndex].boneOffset = bone->mOffsetMatrix;

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

                }
            }


            indexOffset = m_meshVerts.size();
            m_meshBoneWeights.resize(m_meshVerts.size());
        }

        if(m_scene->HasAnimations())
        {
            m_ticksPerSecond = m_scene->mAnimations[0]->mTicksPerSecond;
            m_animationDuration = m_scene->mAnimations[0]->mDuration;
        }
    }



    InitVAO();

}

void RevisionViewer::InitVAO()
{

    m_shaderProg->bind();

    // Get shader locations
    m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_colourLoc = m_shaderProg->uniformLocation("colour");
    glUniform3fv(m_colourLoc, 1, &m_colour[0]);
    m_vertAttrLoc = m_shaderProg->uniformLocation("vertex");
    m_normAttrLoc = m_shaderProg->uniformLocation("normal");
    m_boneIDAttrLoc = m_shaderProg->uniformLocation("boneIDs");
    m_boneWeightAttrLoc = m_shaderProg->uniformLocation("boneWeights");



    // Set up VAO
    m_meshVAO.create();
    m_meshVAO.bind();


    // Set up element array
    m_meshIBO.create();
    m_meshIBO.bind();
    m_meshIBO.allocate(&m_meshTris[0], m_meshTris.size() * sizeof(int));
    m_meshIBO.release();


    // Setup our vertex buffer object.
    m_meshVBO.create();
    m_meshVBO.bind();
    m_meshVBO.allocate(&m_meshVerts[0], m_meshVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO.release();


    // Setup our normals buffer object.
    m_meshNBO.create();
    m_meshNBO.bind();
    m_meshNBO.allocate(&m_meshNorms[0], m_meshNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshNBO.release();


    // Set up vertex bone weighting buffer object
    m_meshBWBO.create();
    m_meshBWBO.bind();
    m_meshBWBO.allocate(&m_meshBoneWeights[0], m_meshBoneWeights.size() * sizeof(VertexBoneData));
    //glEnableVertexAttribArray(m_boneIDAttrLoc);
    //glVertexAttribIPointer(m_boneIDAttrLoc, 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    //glEnableVertexAttribArray(m_boneWeightAttrLoc);
    //glVertexAttribPointer(m_boneWeightAttrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)4);
    m_meshBWBO.release();


    m_meshVAO.release();

    m_shaderProg->release();

}

aiMatrix4x4 RevisionViewer::BoneTransform(float _t, std::vector<aiMatrix4x4> &_transforms)
{
    aiMatrix4x4 identity;

    float timeInTicks = _t * m_ticksPerSecond;
    float animationTime = fmod(timeInTicks, m_animationDuration);

    ReadNodeHierarchy(animationTime, m_scene->mRootNode, identity);


    unsigned int numBones = m_boneInfo.size();
    _transforms.resize(numBones);

    for(unsigned int i=0; i<numBones; i++)
    {
        _transforms[i] = m_boneInfo[i].finalTransform;
    }
}


void RevisionViewer::ReadNodeHierarchy(float _animationTime, const aiNode* _pNode, const aiMatrix4x4 &_parentTransform)
{
    /*
    const aiAnimation* pAnimation = m_scene->mAnimations[0];

    std::string nodeName(_pNode->mName.data);

    aiMatrix4x4 nodeTransform(_pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D scalingVec;
        CalcInterpolatedScaling(scalingVec, _animationTime, pNodeAnim);
        aiMatrix4x4 scalingMat;
        scalingMat.Scaling(scalingVec, scalingMat);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion rotationQ;
        CalcInterpolatedRotation(rotationQ, _animationTime, pNodeAnim);
        aiMatrix4x4 rotationMat = aiMatrix4x4(rotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D translationVec;
        CalcInterpolatedPosition(translationVec, _animationTime, pNodeAnim);
        aiMatrix4x4 translationMat;
        translationMat.Translation(translationVec, translationMat);

        // Combine the above transformations
        nodeTransform = translationMat * rotationMat * scalingMat;
    }

    aiMatrix4x4 globalTransformation = _parentTransform * nodeTransform;

    if (m_boneMapping.find(nodeName) != m_boneMapping.end()) {
        uint BoneIndex = m_boneMapping[nodeName];
        m_boneInfo[BoneIndex].finalTransform = m_globalInverseTransform * globalTransformation * m_boneInfo[BoneIndex].boneOffset;
    }

    for (uint i = 0 ; i < _pNode->mNumChildren ; i++) {
        ReadNodeHierarchy(_animationTime, _pNode->mChildren[i], globalTransformation);
    }
    */
}


