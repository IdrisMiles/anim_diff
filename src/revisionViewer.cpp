#include "include/revisionViewer.h"
#include <iostream>
#include <glm/ext.hpp>
#include <QKeyEvent>


RevisionViewer::RevisionViewer(QWidget *parent) : OpenGLScene(parent)
{
    m_revisionLoaded = false;
    m_initGL = false;
    m_waitingForInitGL = false;

    m_wireframe = false;
    m_drawMesh = true;
    m_playAnim = true;
    m_dt = 0.016;
    m_t = 0.0f;

    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &RevisionViewer::UpdateAnimation);

    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(update()));
}


RevisionViewer::~RevisionViewer()
{
    m_renderModel.m_meshVBO[SKINNED].destroy();
    m_renderModel.m_meshNBO[SKINNED].destroy();
    m_renderModel.m_meshIBO[SKINNED].destroy();
    m_renderModel.m_meshBWBO[SKINNED].destroy();
    m_renderModel.m_meshVAO[SKINNED].destroy();
    cleanup();
}

void RevisionViewer::SetTime(const float _t)
{
    if(m_playAnim)
    {
        m_t = _t;
    }
}

void RevisionViewer::LoadRevision(std::shared_ptr<RevisionNode> _revision)
{
    update();


    m_revision = _revision;
    m_scene = m_revision->m_model->m_scene;
    m_renderModel.m_globalInverseTransform = m_scene->mRootNode->mTransformation;
    m_renderModel.m_globalInverseTransform.Inverse();

    if(!m_initGL)
    {
        std::cout<<"OpenGL has not been initialised yet\n";
        m_waitingForInitGL = true;
        return;
    }

    makeCurrent();
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

    m_revisionLoaded = true;
    doneCurrent();
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
                m_renderModel.m_meshTris.push_back(glm::ivec3(face.mIndices[0]+indexOffset, face.mIndices[1]+indexOffset, face.mIndices[2]+indexOffset));
            }

            // Mesh verts and norms
            unsigned int numVerts = m_scene->mMeshes[i]->mNumVertices;
            for(unsigned int v=0; v<numVerts; v++)
            {
                auto vert = m_scene->mMeshes[i]->mVertices[v];
                auto norm = m_scene->mMeshes[i]->mNormals[v];
                m_renderModel.m_meshVerts.push_back(glm::vec3(vert.x, vert.y, vert.z));
                m_renderModel.m_meshNorms.push_back(glm::vec3(norm.x, norm.y, norm.z));
            }


            m_renderModel.m_meshBoneWeights.resize(m_renderModel.m_meshVerts.size());

            // Mesh bones
            unsigned int numBones = m_scene->mMeshes[i]->mNumBones;
            for(unsigned int b=0; b<numBones; b++)
            {
                auto bone = m_scene->mMeshes[i]->mBones[b];
                unsigned int boneIndex = 0;
                std::string boneName = bone->mName.data;

                // Check this is a new bone
                if(m_renderModel.m_boneMapping.find(boneName) == m_renderModel.m_boneMapping.end())
                {
                    boneIndex = nb;
                    nb++;
                    m_renderModel.m_boneInfo.push_back(BoneTransformData());
                    m_renderModel.m_boneMapping[boneName] = boneIndex;
                    m_renderModel.m_boneInfo[boneIndex].boneOffset = bone->mOffsetMatrix;
                }
                else
                {
                    boneIndex = m_renderModel.m_boneMapping[boneName];

                }


                // Bone vertex weights
                unsigned int boneWeights = bone->mNumWeights;
                for(unsigned int bw=0; bw<boneWeights; bw++)
                {
                    unsigned int vertexID = indexOffset + bone->mWeights[bw].mVertexId;
                    float vertexWeight = bone->mWeights[bw].mWeight;
                    for(unsigned int w=0; w<4; w++)
                    {
                        if(m_renderModel.m_meshBoneWeights[vertexID].boneWeight[w] == 0.0f)
                        {
                            m_renderModel.m_meshBoneWeights[vertexID].boneWeight[w] = vertexWeight;
                            m_renderModel.m_meshBoneWeights[vertexID].boneID[w] = boneIndex;
                        }
                    }
                }

            } // end for numBones

            indexOffset = m_renderModel.m_meshVerts.size();

        } // end for numMeshes

    }// end if has mesh


    if(m_scene->HasAnimations())
    {
        m_animExists = true;
        m_numAnimations = m_scene->mNumAnimations;
        m_animationID = m_numAnimations - 1;
        m_ticksPerSecond = m_scene->mAnimations[m_animationID]->mTicksPerSecond;
        m_animationDuration = m_scene->mAnimations[m_animationID]->mDuration;

        std::cout<<"Animation duration:\t"<<m_animationDuration<<"\n";
        std::cout<<"Number of animations:\t"<<m_scene->mNumAnimations<<"\n";
    }
    else
    {
        m_animExists = false;
        m_playAnim = false;

        for(unsigned int bw=0; bw<m_renderModel.m_meshVerts.size();bw++)
        {
            m_renderModel.m_meshBoneWeights[bw].boneID[0] = 0;
            m_renderModel.m_meshBoneWeights[bw].boneID[1] = 0;
            m_renderModel.m_meshBoneWeights[bw].boneID[2] = 0;
            m_renderModel.m_meshBoneWeights[bw].boneID[3] = 0;

            m_renderModel.m_meshBoneWeights[bw].boneWeight[0] = 1.0;
            m_renderModel.m_meshBoneWeights[bw].boneWeight[1] = 0.0;
            m_renderModel.m_meshBoneWeights[bw].boneWeight[2] = 0.0;
            m_renderModel.m_meshBoneWeights[bw].boneWeight[3] = 0.0;
        }
    }
}

void RevisionViewer::InitRig()
{
    aiMatrix4x4 mat = m_scene->mRootNode->mTransformation * m_renderModel.m_globalInverseTransform;

    for (uint i = 0 ; i < m_scene->mRootNode->mNumChildren ; i++)
    {
        SetRigVerts(m_scene->mRootNode, m_scene->mRootNode->mChildren[i], mat, mat);
    }

    if(m_renderModel.m_rigVerts.size() % 2)
    {
        for(unsigned int i=0; i<m_renderModel.m_rigVerts.size()/2; i++)
        {
            int id = i*2;
            if(m_renderModel.m_rigVerts[id] == m_renderModel.m_rigVerts[id+1])
            {
                std::cout<<"Repeated joint causing rig issue, removing joint\n";
                m_renderModel.m_rigVerts.erase(m_renderModel.m_rigVerts.begin()+id);
                m_renderModel.m_rigJointColours.erase(m_renderModel.m_rigJointColours.begin()+id);
                m_renderModel.m_rigBoneWeights.erase(m_renderModel.m_rigBoneWeights.begin()+id);

                break;
            }
        }
    }

    std::cout<<"Number of rig verts:\t"<<m_renderModel.m_rigVerts.size()<<"\n";

}


void RevisionViewer::SetRigVerts(aiNode* _pParentNode, aiNode* _pNode, const aiMatrix4x4 &_parentTransform, const aiMatrix4x4 &_thisTransform)
{
    const std::string parentNodeName(_pParentNode->mName.data);
    const std::string nodeName = _pNode->mName.data;
    bool isBone = m_renderModel.m_boneMapping.find(nodeName) != m_renderModel.m_boneMapping.end();

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

void RevisionViewer::SetJointVert(const std::string _nodeName, const aiMatrix4x4 &_transform, VertexBoneData &_vb)
{
    if(m_renderModel.m_boneMapping.find(_nodeName) != m_renderModel.m_boneMapping.end())
    {
        _vb.boneID[0] = m_renderModel.m_boneMapping[_nodeName];
        _vb.boneWeight[0] = 1.0f;
        _vb.boneWeight[1] = 0.0f;
        _vb.boneWeight[2] = 0.0f;
        _vb.boneWeight[3] = 0.0f;

        m_renderModel.m_rigVerts.push_back(glm::vec3(glm::vec4(0.0f,0.0f,0.0f,1.0f)*ViewerUtilities::ConvertToGlmMat(_transform)));
        m_renderModel.m_rigJointColours.push_back(glm::vec3(0.4f, 1.0f, 0.4f));
        m_renderModel.m_rigBoneWeights.push_back(_vb);
    }
    else
    {
        std::cout<<"This Node is not a bone, skipping\n";
    }

}



void RevisionViewer::InitAnimation()
{

}

void RevisionViewer::InitVAO()
{

    glPointSize(10);

    //--------------------------------------------------------------------------------------
    // Skinned mesh
    m_renderModel.m_shaderProg[SKINNED]->bind();

    // Get shader locations
    m_renderModel.m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_renderModel.m_colourLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->uniformLocation("uColour");
    glUniform3fv(m_renderModel.m_colourLoc[SKINNED], 1, &m_renderModel.m_colour[0]);
    m_renderModel.m_vertAttrLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->attributeLocation("vertex");
    m_renderModel.m_normAttrLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->attributeLocation("normal");
    m_renderModel.m_boneIDAttrLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->attributeLocation("BoneIDs");
    m_renderModel.m_boneWeightAttrLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->attributeLocation("Weights");
    m_renderModel.m_boneUniformLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->uniformLocation("Bones");

    std::cout<<"SKINNED | vert Attr loc:\t"<<m_renderModel.m_vertAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | norm Attr loc:\t"<<m_renderModel.m_normAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | boneID Attr loc:\t"<<m_renderModel.m_boneIDAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | boneWei Attr loc:\t"<<m_renderModel.m_boneWeightAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | Bones Unif loc:\t"<<m_renderModel.m_boneUniformLoc[SKINNED]<<"\n";


    // Set up VAO
    m_renderModel.m_meshVAO[SKINNED].create();
    m_renderModel.m_meshVAO[SKINNED].bind();

    // Set up element array
    m_renderModel.m_meshIBO[SKINNED].create();
    m_renderModel.m_meshIBO[SKINNED].bind();
    m_renderModel.m_meshIBO[SKINNED].allocate(&m_renderModel.m_meshTris[0], m_renderModel.m_meshTris.size() * sizeof(int));
    m_renderModel.m_meshIBO[SKINNED].release();


    // Setup our vertex buffer object.
    m_renderModel.m_meshVBO[SKINNED].create();
    m_renderModel.m_meshVBO[SKINNED].bind();
    m_renderModel.m_meshVBO[SKINNED].allocate(&m_renderModel.m_meshVerts[0], m_renderModel.m_meshVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_renderModel.m_vertAttrLoc[SKINNED]);
    glVertexAttribPointer(m_renderModel.m_vertAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_renderModel.m_meshVBO[SKINNED].release();


    // Setup our normals buffer object.
    m_renderModel.m_meshNBO[SKINNED].create();
    m_renderModel.m_meshNBO[SKINNED].bind();
    m_renderModel.m_meshNBO[SKINNED].allocate(&m_renderModel.m_meshNorms[0], m_renderModel.m_meshNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_renderModel.m_normAttrLoc[SKINNED]);
    glVertexAttribPointer(m_renderModel.m_normAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_renderModel.m_meshNBO[SKINNED].release();


    // Set up vertex bone weighting buffer object
    m_renderModel.m_meshBWBO[SKINNED].create();
    m_renderModel.m_meshBWBO[SKINNED].bind();
    m_renderModel.m_meshBWBO[SKINNED].allocate(&m_renderModel.m_meshBoneWeights[0], m_renderModel.m_meshBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_renderModel.m_boneIDAttrLoc[SKINNED]);
    glVertexAttribIPointer(m_renderModel.m_boneIDAttrLoc[SKINNED], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_renderModel.m_boneWeightAttrLoc[SKINNED]);
    glVertexAttribPointer(m_renderModel.m_boneWeightAttrLoc[SKINNED], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(4*sizeof(unsigned int)));
    m_renderModel.m_meshBWBO[SKINNED].release();

    m_renderModel.m_meshVAO[SKINNED].release();
    m_renderModel.m_shaderProg[SKINNED]->release();



    //--------------------------------------------------------------------------------------
    // Rigged mesh
    m_renderModel.m_shaderProg[RIG]->bind();

    // Get shader locations
    m_renderModel.m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_renderModel.m_vertAttrLoc[RIG] = m_renderModel.m_shaderProg[RIG]->attributeLocation("vertex");
    m_renderModel.m_boneIDAttrLoc[RIG] = m_renderModel.m_shaderProg[RIG]->attributeLocation("BoneIDs");
    m_renderModel.m_boneWeightAttrLoc[RIG] = m_renderModel.m_shaderProg[RIG]->attributeLocation("Weights");
    m_renderModel.m_boneUniformLoc[RIG] = m_renderModel.m_shaderProg[RIG]->uniformLocation("Bones");
    m_renderModel.m_colourAttrLoc[RIG] = m_renderModel.m_shaderProg[RIG]->attributeLocation("colour");


    std::cout<<"RIG | vert Attr loc:\t"<<m_renderModel.m_vertAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | boneID Attr loc:\t"<<m_renderModel.m_boneIDAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | boneWei Attr loc:\t"<<m_renderModel.m_boneWeightAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | Bones Unif loc:\t"<<m_renderModel.m_boneUniformLoc[RIG]<<"\n";
    std::cout<<"RIG | colour Attr loc: "<<m_renderModel.m_colourAttrLoc[RIG]<<"\n";


    m_renderModel.m_meshVAO[RIG].create();
    m_renderModel.m_meshVAO[RIG].bind();

    // Setup our vertex buffer object.
    m_renderModel.m_meshVBO[RIG].create();
    m_renderModel.m_meshVBO[RIG].bind();
    m_renderModel.m_meshVBO[RIG].allocate(&m_renderModel.m_rigVerts[0], m_renderModel.m_rigVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_renderModel.m_vertAttrLoc[RIG]);
    glVertexAttribPointer(m_renderModel.m_vertAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_renderModel.m_meshVBO[RIG].release();

    // Set up our Rig joint colour buffer object
    m_renderModel.m_meshCBO[RIG].create();
    m_renderModel.m_meshCBO[RIG].bind();
    m_renderModel.m_meshCBO[RIG].allocate(&m_renderModel.m_rigJointColours[0], m_renderModel.m_rigJointColours.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_renderModel.m_colourAttrLoc[RIG]);
    glVertexAttribPointer(m_renderModel.m_colourAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_renderModel.m_meshCBO[RIG].release();

    // Set up vertex bone weighting buffer object
    m_renderModel.m_meshBWBO[RIG].create();
    m_renderModel.m_meshBWBO[RIG].bind();
    m_renderModel.m_meshBWBO[RIG].allocate(&m_renderModel.m_rigBoneWeights[0], m_renderModel.m_rigBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_renderModel.m_boneIDAttrLoc[RIG]);
    glVertexAttribIPointer(m_renderModel.m_boneIDAttrLoc[RIG], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_renderModel.m_boneWeightAttrLoc[RIG]);
    glVertexAttribPointer(m_renderModel.m_boneWeightAttrLoc[RIG], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*) (4*sizeof(unsigned int)));
    m_renderModel.m_meshBWBO[RIG].release();

    m_renderModel.m_meshVAO[RIG].release();

    m_renderModel.m_shaderProg[RIG]->release();

}

void RevisionViewer::customInitGL()
{
    // initialise view and projection matrices
    m_viewMat = glm::mat4(1);
    m_viewMat = glm::lookAt(glm::vec3(0,0,0),glm::vec3(0,0,-1),glm::vec3(0,1,0));
    m_projMat = glm::perspective(45.0f, GLfloat(width()) / height(), 0.01f, 2000.0f);

    //------------------------------------------------------------------------------------------------
    // SKINNING Shader
    m_renderModel.m_shaderProg[SKINNED] = new QOpenGLShaderProgram();
    m_renderModel.m_shaderProg[SKINNED]->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/skinningVert.glsl");
    m_renderModel.m_shaderProg[SKINNED]->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/skinningFrag.glsl");
    m_renderModel.m_shaderProg[SKINNED]->bindAttributeLocation("vertex", 0);
    m_renderModel.m_shaderProg[SKINNED]->bindAttributeLocation("normal", 1);
    m_renderModel.m_shaderProg[SKINNED]->link();

    m_renderModel.m_shaderProg[SKINNED]->bind();
    m_renderModel.m_projMatrixLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->uniformLocation("projMatrix");
    m_renderModel.m_mvMatrixLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->uniformLocation("mvMatrix");
    m_renderModel.m_normalMatrixLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->uniformLocation("normalMatrix");
    m_renderModel.m_lightPosLoc[SKINNED] = m_renderModel.m_shaderProg[SKINNED]->uniformLocation("lightPos");

    // Light position is fixed.
    m_lightPos = glm::vec3(0, 0, 70);
    glUniform3fv(m_renderModel.m_lightPosLoc[SKINNED], 1, &m_lightPos[0]);
    m_renderModel.m_shaderProg[SKINNED]->release();


    //------------------------------------------------------------------------------------------------
    // RIG Shader
    m_renderModel.m_shaderProg[RIG] = new QOpenGLShaderProgram();
    m_renderModel.m_shaderProg[RIG]->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/rigVert.glsl");
    m_renderModel.m_shaderProg[RIG]->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/rigFrag.glsl");
    m_renderModel.m_shaderProg[RIG]->addShaderFromSourceFile(QOpenGLShader::Geometry, "../shader/rigGeo.glsl");
    m_renderModel.m_shaderProg[RIG]->bindAttributeLocation("vertex", 0);
    m_renderModel.m_shaderProg[RIG]->link();

    m_renderModel.m_shaderProg[RIG]->bind();
    m_renderModel.m_projMatrixLoc[RIG] = m_renderModel.m_shaderProg[RIG]->uniformLocation("projMatrix");
    m_renderModel.m_mvMatrixLoc[RIG] = m_renderModel.m_shaderProg[RIG]->uniformLocation("mvMatrix");
    m_renderModel.m_normalMatrixLoc[RIG] = m_renderModel.m_shaderProg[RIG]->uniformLocation("normalMatrix");
    m_renderModel.m_shaderProg[RIG]->release();

    m_initGL = true;

}

void RevisionViewer::paintGL()
{
    if(m_waitingForInitGL)
    {
        LoadRevision(m_revision);
        m_waitingForInitGL = false;
    }

    // clean gl window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if(!m_revisionLoaded)
    {
        return;
    }

    // update model matrix
    m_modelMat = glm::mat4(1);
    m_modelMat = glm::translate(m_modelMat, glm::vec3(0, 0, -0.1f*m_zDis));// m_zDis));
    m_modelMat = glm::rotate(m_modelMat, glm::radians(m_xRot/16.0f), glm::vec3(1,0,0));
    m_modelMat = glm::rotate(m_modelMat, glm::radians(m_yRot/16.0f), glm::vec3(0,1,0));
    //m_modelMat = glm::translate(m_modelMat, glm::vec3(0.1f*m_xDis, -0.1f*m_yDis, 0));


    // Set shader params
    m_renderModel.m_shaderProg[SKINNED]->bind();
    glUniformMatrix4fv(m_renderModel.m_projMatrixLoc[SKINNED], 1, false, &m_projMat[0][0]);
    glUniformMatrix4fv(m_renderModel.m_mvMatrixLoc[SKINNED], 1, false, &(m_modelMat*m_viewMat)[0][0]);
    glm::mat3 normalMatrix =  glm::inverse(glm::mat3(m_modelMat));
    glUniformMatrix3fv(m_renderModel.m_normalMatrixLoc[SKINNED], 1, true, &normalMatrix[0][0]);
    glUniform3fv(m_renderModel.m_colourLoc[SKINNED], 1, &m_renderModel.m_colour[0]);

    //---------------------------------------------------------------------------------------
    // Draw code - replace this with project specific draw stuff
    DrawMesh();
    //---------------------------------------------------------------------------------------
    m_renderModel.m_shaderProg[SKINNED]->release();



    if(m_wireframe)
    {
        m_renderModel.m_shaderProg[RIG]->bind();
        glUniformMatrix4fv(m_renderModel.m_projMatrixLoc[RIG], 1, false, &m_projMat[0][0]);
        glUniformMatrix4fv(m_renderModel.m_mvMatrixLoc[RIG], 1, false, &(m_modelMat*m_viewMat)[0][0]);
        glUniformMatrix3fv(m_renderModel.m_normalMatrixLoc[RIG], 1, true, &normalMatrix[0][0]);

        DrawRig();

        m_renderModel.m_shaderProg[RIG]->release();
    }
}

void RevisionViewer::DrawMesh()
{
    m_renderModel.m_meshVAO[SKINNED].bind();
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe?GL_LINE:GL_FILL);
    glDrawElements(GL_TRIANGLES, 3*m_renderModel.m_meshTris.size(), GL_UNSIGNED_INT, &m_renderModel.m_meshTris[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_renderModel.m_meshVAO[SKINNED].release();
}

void RevisionViewer::DrawRig()
{
    m_renderModel.m_meshVAO[RIG].bind();
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe?GL_LINE:GL_FILL);
    glDrawArrays(GL_LINES, 0, m_renderModel.m_rigVerts.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_renderModel.m_meshVAO[RIG].release();
}

void RevisionViewer::UpdateAnimation()
{

    // Set shader params
    m_renderModel.m_shaderProg[SKINNED]->bind();
    UploadBonesToShader(m_t, SKINNED);
    m_renderModel.m_shaderProg[SKINNED]->release();

    m_renderModel.m_shaderProg[RIG]->bind();
    UploadBonesToShader(m_t, RIG);
    m_renderModel.m_shaderProg[RIG]->release();
}

void RevisionViewer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        m_wireframe = !m_wireframe;
    }
}

void RevisionViewer::UploadBonesToShader(const float _t, RenderType _rt)
{
    std::vector<glm::mat4> bones;
    BoneTransform(_t, bones);
    for(unsigned int b=0; b<bones.size() && b<100; b++)
    {
        glUniformMatrix4fv(m_renderModel.m_boneUniformLoc[_rt] + b, 1, true, &bones[b][0][0]);
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
//    std::cout<<"Orig Time:\t"<<_t<<"\n";
//    std::cout<<"Time in Ticks:\t"<<timeInTicks<<"\n";
//    std::cout<<"Anim Time:\t"<<animationTime<<"\n";

    ViewerUtilities::ReadNodeHierarchy(m_renderModel.m_boneMapping, m_renderModel.m_boneInfo, m_renderModel.m_globalInverseTransform, animationTime, m_scene->mAnimations[m_animationID], m_scene->mRootNode, identity);

    unsigned int numBones = m_renderModel.m_boneInfo.size();
    _transforms.resize(numBones);

    for(unsigned int i=0; i<numBones; i++)
    {
        _transforms[i] = ViewerUtilities::ConvertToGlmMat(m_renderModel.m_boneInfo[i].finalTransform);
    }

}
