#include "revisionViewer.h"
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
    m_model->m_meshVBO[SKINNED].destroy();
    m_model->m_meshNBO[SKINNED].destroy();
    m_model->m_meshIBO[SKINNED].destroy();
    m_model->m_meshBWBO[SKINNED].destroy();
    m_model->m_meshVAO[SKINNED].destroy();
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
    m_model = m_revision->m_model;

    if(!m_initGL)
    {
        std::cout<<"OpenGL has not been initialised yet\n";
        m_waitingForInitGL = true;
        return;
    }

    makeCurrent();

    InitVAO();

    m_animTimer->start(1000*m_dt);
    m_drawTimer->start(1000*m_dt);

    m_revisionLoaded = true;
    doneCurrent();
}


void RevisionViewer::InitVAO()
{

    glPointSize(10);

    //--------------------------------------------------------------------------------------
    // Skinned mesh
    m_model->m_shaderProg[SKINNED]->bind();

    // Get shader locations
    m_model->m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_model->m_colourLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->uniformLocation("uColour");
    glUniform3fv(m_model->m_colourLoc[SKINNED], 1, &m_model->m_colour[0]);
    m_model->m_vertAttrLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->attributeLocation("vertex");
    m_model->m_normAttrLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->attributeLocation("normal");
    m_model->m_boneIDAttrLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->attributeLocation("BoneIDs");
    m_model->m_boneWeightAttrLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->attributeLocation("Weights");
    m_model->m_boneUniformLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->uniformLocation("Bones");

    std::cout<<"SKINNED | vert Attr loc:\t"<<m_model->m_vertAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | norm Attr loc:\t"<<m_model->m_normAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | boneID Attr loc:\t"<<m_model->m_boneIDAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | boneWei Attr loc:\t"<<m_model->m_boneWeightAttrLoc[SKINNED]<<"\n";
    std::cout<<"SKINNED | Bones Unif loc:\t"<<m_model->m_boneUniformLoc[SKINNED]<<"\n";


    // Set up VAO
    m_model->m_meshVAO[SKINNED].create();
    m_model->m_meshVAO[SKINNED].bind();

    // Set up element array
    m_model->m_meshIBO[SKINNED].create();
    m_model->m_meshIBO[SKINNED].bind();
    m_model->m_meshIBO[SKINNED].allocate(&m_model->m_meshTris[0], m_model->m_meshTris.size() * sizeof(int));
    m_model->m_meshIBO[SKINNED].release();


    // Setup our vertex buffer object.
    m_model->m_meshVBO[SKINNED].create();
    m_model->m_meshVBO[SKINNED].bind();
    m_model->m_meshVBO[SKINNED].allocate(&m_model->m_meshVerts[0], m_model->m_meshVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_model->m_vertAttrLoc[SKINNED]);
    glVertexAttribPointer(m_model->m_vertAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_model->m_meshVBO[SKINNED].release();


    // Setup our normals buffer object.
    m_model->m_meshNBO[SKINNED].create();
    m_model->m_meshNBO[SKINNED].bind();
    m_model->m_meshNBO[SKINNED].allocate(&m_model->m_meshNorms[0], m_model->m_meshNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_model->m_normAttrLoc[SKINNED]);
    glVertexAttribPointer(m_model->m_normAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_model->m_meshNBO[SKINNED].release();


    // Set up vertex bone weighting buffer object
    m_model->m_meshBWBO[SKINNED].create();
    m_model->m_meshBWBO[SKINNED].bind();
    m_model->m_meshBWBO[SKINNED].allocate(&m_model->m_meshBoneWeights[0], m_model->m_meshBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_model->m_boneIDAttrLoc[SKINNED]);
    glVertexAttribIPointer(m_model->m_boneIDAttrLoc[SKINNED], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_model->m_boneWeightAttrLoc[SKINNED]);
    glVertexAttribPointer(m_model->m_boneWeightAttrLoc[SKINNED], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(4*sizeof(unsigned int)));
    m_model->m_meshBWBO[SKINNED].release();

    m_model->m_meshVAO[SKINNED].release();
    m_model->m_shaderProg[SKINNED]->release();



    //--------------------------------------------------------------------------------------
    // Rigged mesh
    m_model->m_shaderProg[RIG]->bind();

    // Get shader locations
    m_model->m_colour = glm::vec3(0.8f,0.4f,0.4f);
    m_model->m_vertAttrLoc[RIG] = m_model->m_shaderProg[RIG]->attributeLocation("vertex");
    m_model->m_boneIDAttrLoc[RIG] = m_model->m_shaderProg[RIG]->attributeLocation("BoneIDs");
    m_model->m_boneWeightAttrLoc[RIG] = m_model->m_shaderProg[RIG]->attributeLocation("Weights");
    m_model->m_boneUniformLoc[RIG] = m_model->m_shaderProg[RIG]->uniformLocation("Bones");
    m_model->m_colourAttrLoc[RIG] = m_model->m_shaderProg[RIG]->attributeLocation("colour");


    std::cout<<"RIG | vert Attr loc:\t"<<m_model->m_vertAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | boneID Attr loc:\t"<<m_model->m_boneIDAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | boneWei Attr loc:\t"<<m_model->m_boneWeightAttrLoc[RIG]<<"\n";
    std::cout<<"RIG | Bones Unif loc:\t"<<m_model->m_boneUniformLoc[RIG]<<"\n";
    std::cout<<"RIG | colour Attr loc: "<<m_model->m_colourAttrLoc[RIG]<<"\n";


    m_model->m_meshVAO[RIG].create();
    m_model->m_meshVAO[RIG].bind();

    // Setup our vertex buffer object.
    m_model->m_meshVBO[RIG].create();
    m_model->m_meshVBO[RIG].bind();
    m_model->m_meshVBO[RIG].allocate(&m_model->m_rigVerts[0], m_model->m_rigVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_model->m_vertAttrLoc[RIG]);
    glVertexAttribPointer(m_model->m_vertAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_model->m_meshVBO[RIG].release();

    // Set up our Rig joint colour buffer object
    m_model->m_meshCBO[RIG].create();
    m_model->m_meshCBO[RIG].bind();
    m_model->m_meshCBO[RIG].allocate(&m_model->m_rigJointColours[0], m_model->m_rigJointColours.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_model->m_colourAttrLoc[RIG]);
    glVertexAttribPointer(m_model->m_colourAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_model->m_meshCBO[RIG].release();

    // Set up vertex bone weighting buffer object
    m_model->m_meshBWBO[RIG].create();
    m_model->m_meshBWBO[RIG].bind();
    m_model->m_meshBWBO[RIG].allocate(&m_model->m_rigBoneWeights[0], m_model->m_rigBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_model->m_boneIDAttrLoc[RIG]);
    glVertexAttribIPointer(m_model->m_boneIDAttrLoc[RIG], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_model->m_boneWeightAttrLoc[RIG]);
    glVertexAttribPointer(m_model->m_boneWeightAttrLoc[RIG], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*) (4*sizeof(unsigned int)));
    m_model->m_meshBWBO[RIG].release();

    m_model->m_meshVAO[RIG].release();

    m_model->m_shaderProg[RIG]->release();

}

void RevisionViewer::customInitGL()
{
    // initialise view and projection matrices
    m_viewMat = glm::mat4(1);
    m_viewMat = glm::lookAt(glm::vec3(0,0,0),glm::vec3(0,0,-1),glm::vec3(0,1,0));
    m_projMat = glm::perspective(45.0f, GLfloat(width()) / height(), 0.01f, 2000.0f);

    //------------------------------------------------------------------------------------------------
    // SKINNING Shader
    m_model->m_shaderProg[SKINNED] = new QOpenGLShaderProgram();
    m_model->m_shaderProg[SKINNED]->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/skinningVert.glsl");
    m_model->m_shaderProg[SKINNED]->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/skinningFrag.glsl");
    m_model->m_shaderProg[SKINNED]->bindAttributeLocation("vertex", 0);
    m_model->m_shaderProg[SKINNED]->bindAttributeLocation("normal", 1);
    m_model->m_shaderProg[SKINNED]->link();

    m_model->m_shaderProg[SKINNED]->bind();
    m_model->m_projMatrixLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->uniformLocation("projMatrix");
    m_model->m_mvMatrixLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->uniformLocation("mvMatrix");
    m_model->m_normalMatrixLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->uniformLocation("normalMatrix");
    m_model->m_lightPosLoc[SKINNED] = m_model->m_shaderProg[SKINNED]->uniformLocation("lightPos");

    // Light position is fixed.
    m_lightPos = glm::vec3(0, 0, 70);
    glUniform3fv(m_model->m_lightPosLoc[SKINNED], 1, &m_lightPos[0]);
    m_model->m_shaderProg[SKINNED]->release();


    //------------------------------------------------------------------------------------------------
    // RIG Shader
    m_model->m_shaderProg[RIG] = new QOpenGLShaderProgram();
    m_model->m_shaderProg[RIG]->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shader/rigVert.glsl");
    m_model->m_shaderProg[RIG]->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shader/rigFrag.glsl");
    m_model->m_shaderProg[RIG]->addShaderFromSourceFile(QOpenGLShader::Geometry, "../shader/rigGeo.glsl");
    m_model->m_shaderProg[RIG]->bindAttributeLocation("vertex", 0);
    m_model->m_shaderProg[RIG]->link();

    m_model->m_shaderProg[RIG]->bind();
    m_model->m_projMatrixLoc[RIG] = m_model->m_shaderProg[RIG]->uniformLocation("projMatrix");
    m_model->m_mvMatrixLoc[RIG] = m_model->m_shaderProg[RIG]->uniformLocation("mvMatrix");
    m_model->m_normalMatrixLoc[RIG] = m_model->m_shaderProg[RIG]->uniformLocation("normalMatrix");
    m_model->m_shaderProg[RIG]->release();

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
    m_model->m_shaderProg[SKINNED]->bind();
    glUniformMatrix4fv(m_model->m_projMatrixLoc[SKINNED], 1, false, &m_projMat[0][0]);
    glUniformMatrix4fv(m_model->m_mvMatrixLoc[SKINNED], 1, false, &(m_modelMat*m_viewMat)[0][0]);
    glm::mat3 normalMatrix =  glm::inverse(glm::mat3(m_modelMat));
    glUniformMatrix3fv(m_model->m_normalMatrixLoc[SKINNED], 1, true, &normalMatrix[0][0]);
    glUniform3fv(m_model->m_colourLoc[SKINNED], 1, &m_model->m_colour[0]);

    //---------------------------------------------------------------------------------------
    // Draw code - replace this with project specific draw stuff
    DrawMesh();
    m_model->m_shaderProg[SKINNED]->release();


    if(m_wireframe)
    {
        m_model->m_shaderProg[RIG]->bind();
        glUniformMatrix4fv(m_model->m_projMatrixLoc[RIG], 1, false, &m_projMat[0][0]);
        glUniformMatrix4fv(m_model->m_mvMatrixLoc[RIG], 1, false, &(m_modelMat*m_viewMat)[0][0]);
        glUniformMatrix3fv(m_model->m_normalMatrixLoc[RIG], 1, true, &normalMatrix[0][0]);

        DrawRig();

        m_model->m_shaderProg[RIG]->release();
    }
}

void RevisionViewer::DrawMesh()
{
    m_model->m_meshVAO[SKINNED].bind();
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe?GL_LINE:GL_FILL);
    glDrawElements(GL_TRIANGLES, 3*m_model->m_meshTris.size(), GL_UNSIGNED_INT, &m_model->m_meshTris[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_model->m_meshVAO[SKINNED].release();
}

void RevisionViewer::DrawRig()
{
    m_model->m_meshVAO[RIG].bind();
    glPolygonMode(GL_FRONT_AND_BACK, m_wireframe?GL_LINE:GL_FILL);
    glDrawArrays(GL_LINES, 0, m_model->m_rigVerts.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_model->m_meshVAO[RIG].release();
}

void RevisionViewer::UpdateAnimation()
{

    // Set shader params
    m_model->m_shaderProg[SKINNED]->bind();
    UploadBonesToShader(m_t, SKINNED);
    m_model->m_shaderProg[SKINNED]->release();

    m_model->m_shaderProg[RIG]->bind();
    UploadBonesToShader(m_t, RIG);
    m_model->m_shaderProg[RIG]->release();
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
        glUniformMatrix4fv(m_model->m_boneUniformLoc[_rt] + b, 1, true, &bones[b][0][0]);
    }
}

void RevisionViewer::BoneTransform(const float _t, std::vector<glm::mat4> &_transforms)
{
    glm::mat4 identity;

    if(!m_model->m_animExists)
    {
        _transforms.resize(1);
        _transforms[0] = glm::mat4(1.0);
        return;
    }

    float timeInTicks = _t * m_model->m_ticksPerSecond;
    float animationTime = fmod(timeInTicks, m_model->m_animationDuration);

    ViewerUtilities::ReadNodeHierarchy(m_model->m_globalInverseTransform, animationTime, m_model->m_rig, m_model->m_rig.m_rootBone, identity);

    unsigned int numBones = m_model->m_boneInfo.size();
    _transforms.resize(numBones);

    for(unsigned int i=0; i<numBones; i++)
    {
        _transforms[i] = ViewerUtilities::ConvertToGlmMat(m_model->m_boneInfo[i].finalTransform);
    }

}
