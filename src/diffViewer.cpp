#include "include/diffViewer.h"
#include <QKeyEvent>
#include <iostream>



DiffViewer::DiffViewer(QWidget *parent) : RevisionViewer(parent)
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
    connect(m_animTimer, &QTimer::timeout, this, &DiffViewer::UpdateAnimation);

    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(update()));
}


DiffViewer::~DiffViewer()
{
    if(m_animTimer != nullptr)
    {
        delete m_animTimer;
        m_animTimer = nullptr;
    }

    if(m_drawTimer != nullptr)
    {
        delete m_drawTimer;
        m_drawTimer = nullptr;
    }

    m_revisionDiff = nullptr;
    m_modelDiff = nullptr;

    m_revision  = nullptr;
    m_model = nullptr;

    DeleteVAOs();
    DeleteShaders();

    cleanup();

}

void DiffViewer::LoadDiff(std::shared_ptr<RevisionDiff> _diff)
{
    std::cout<<"Loading a Merged Revision\n";
    update();


    m_revisionDiff = _diff;
    m_revision = m_revisionDiff->getMasterNode();
    m_model = m_revision->m_model;
    m_modelDiff = m_revisionDiff->getBranchNode()->m_model;

    if(!m_initGL)
    {
        std::cout<<"OpenGL has not been initialised yet\n";
        m_waitingForInitGL = true;
        return;
    }

    makeCurrent();
    UpdateVAOs();
    doneCurrent();

    m_animTimer->start(1000*m_dt);
    m_drawTimer->start(1000*m_dt);

    m_revisionLoaded = true;
}

void DiffViewer::UpdateVAOs()
{
    glPointSize(10);

    //--------------------------------------------------------------------------------------
    // Skinned mesh
    m_shaderProg[SKINNED]->bind();


    // Set up VAO
    m_meshVAO[SKINNED].bind();

    // Set up element array
    m_meshIBO[SKINNED].bind();
    m_meshIBO[SKINNED].allocate(&m_model->m_meshTris[0], m_model->m_meshTris.size() * sizeof(int));
    m_meshIBO[SKINNED].release();


    // Setup our vertex buffer object.
    m_meshVBO[SKINNED].bind();
    m_meshVBO[SKINNED].allocate(&m_model->m_meshVerts[0], m_model->m_meshVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_vertAttrLoc[SKINNED]);
    glVertexAttribPointer(m_vertAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO[SKINNED].release();


    // Setup our normals buffer object.
    m_meshNBO[SKINNED].bind();
    m_meshNBO[SKINNED].allocate(&m_model->m_meshNorms[0], m_model->m_meshNorms.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_normAttrLoc[SKINNED]);
    glVertexAttribPointer(m_normAttrLoc[SKINNED], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshNBO[SKINNED].release();


    // Set up vertex bone weighting buffer object
    m_meshBWBO[SKINNED].bind();
    m_meshBWBO[SKINNED].allocate(&m_model->m_meshBoneWeights[0], m_model->m_meshBoneWeights.size() * sizeof(VertexBoneData));
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


    m_meshVAO[RIG].bind();

    // Setup our vertex buffer object.
    m_meshVBO[RIG].bind();
    m_meshVBO[RIG].allocate(&m_model->m_rigVerts[0], m_model->m_rigVerts.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_vertAttrLoc[RIG]);
    glVertexAttribPointer(m_vertAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshVBO[RIG].release();

    // Set up our Rig joint colour buffer object
    m_meshCBO[RIG].bind();
    m_meshCBO[RIG].allocate(&m_model->m_rigJointColours[0], m_model->m_rigJointColours.size() * sizeof(glm::vec3));
    glEnableVertexAttribArray(m_colourAttrLoc[RIG]);
    glVertexAttribPointer(m_colourAttrLoc[RIG], 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    m_meshCBO[RIG].release();

    // Set up vertex bone weighting buffer object
    m_meshBWBO[RIG].bind();
    m_meshBWBO[RIG].allocate(&m_model->m_rigBoneWeights[0], m_model->m_rigBoneWeights.size() * sizeof(VertexBoneData));
    glEnableVertexAttribArray(m_boneIDAttrLoc[RIG]);
    glVertexAttribIPointer(m_boneIDAttrLoc[RIG], 4, GL_UNSIGNED_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(m_boneWeightAttrLoc[RIG]);
    glVertexAttribPointer(m_boneWeightAttrLoc[RIG], 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*) (4*sizeof(unsigned int)));
    m_meshBWBO[RIG].release();

    m_meshVAO[RIG].release();

    m_shaderProg[RIG]->release();
}

void DiffViewer::customInitGL()
{
    // initialise view and projection matrices
    m_viewMat = glm::mat4(1);
    m_viewMat = glm::lookAt(glm::vec3(0,0,0),glm::vec3(0,0,-1),glm::vec3(0,1,0));
    m_projMat = glm::perspective(45.0f, GLfloat(width()) / height(), 0.01f, 2000.0f);

    CreateShaders();
    CreateVAOs();
    m_colourAttrLoc[SKINNED] = m_shaderProg[SKINNED]->uniformLocation("BoneColours");

    m_initGL = true;
}

void DiffViewer::paintGL()
{
    if(m_waitingForInitGL)
    {
        LoadDiff(m_revisionDiff);
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
    m_shaderProg[SKINNED]->bind();
    glUniformMatrix4fv(m_projMatrixLoc[SKINNED], 1, false, &m_projMat[0][0]);
    glUniformMatrix4fv(m_mvMatrixLoc[SKINNED], 1, false, &(m_modelMat*m_viewMat)[0][0]);
    glm::mat3 normalMatrix =  glm::inverse(glm::mat3(m_modelMat));
    glUniformMatrix3fv(m_normalMatrixLoc[SKINNED], 1, true, &normalMatrix[0][0]);
    glUniform3fv(m_colourLoc[SKINNED], 1, &m_model->m_colour[0]);

    //---------------------------------------------------------------------------------------
    // Draw code - replace this with project specific draw stuff
    DrawMesh();
    m_shaderProg[SKINNED]->release();


    if(m_wireframe)
    {
        m_shaderProg[RIG]->bind();
        glUniformMatrix4fv(m_projMatrixLoc[RIG], 1, false, &m_projMat[0][0]);
        glUniformMatrix4fv(m_mvMatrixLoc[RIG], 1, false, &(m_modelMat*m_viewMat)[0][0]);
        glUniformMatrix3fv(m_normalMatrixLoc[RIG], 1, true, &normalMatrix[0][0]);

        DrawRig();

        m_shaderProg[RIG]->release();
    }
}

void DiffViewer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        m_wireframe = !m_wireframe;
    }
}

void DiffViewer::UpdateAnimation()
{
    float timeInTicks = m_t * m_model->m_ticksPerSecond;
    float animationTime = fmod(timeInTicks, m_model->m_animationDuration);

    // Compute bone transform
    std::vector<glm::mat4> boneTrans;
    ComputeBoneTransform(animationTime, boneTrans);
    ComputeBoneColours(animationTime);

    // Upload bone transforms to shaders
    m_shaderProg[SKINNED]->bind();
    UploadBonesToShader(boneTrans, SKINNED);
    UploadBoneColoursToShader(m_model->m_rigJointColours, SKINNED);
    m_shaderProg[SKINNED]->release();

    m_shaderProg[RIG]->bind();
    UploadBonesToShader(boneTrans, RIG);
    UploadBoneColoursToShader(m_model->m_rigJointColours, SKINNED);
    m_shaderProg[RIG]->release();
}

void DiffViewer::UploadBoneColoursToShader(std::vector<glm::vec3> &_rigJointColour, RenderType _rt)
{
    for(unsigned int b=0; b<_rigJointColour.size() && b<100; b++)
    {
        glUniform3fv(m_colourAttrLoc[_rt] + b, 1, &_rigJointColour[b][0] );
    }
}

void DiffViewer::ComputeBoneColours(const float _t)
{
    ViewerUtilities::ColourBoneDifferences(m_model->m_boneMapping, _t, m_revisionDiff->getBoneDeltas(), m_model->m_rig, m_model->m_rig->m_rootBone, m_revisionDiff->getDiffRig(), m_model->m_rigJointColours);
}

void DiffViewer::ComputeBoneTransform(const float _t, std::vector<glm::mat4> &_transforms)
{
    glm::mat4 identity;

    if(!m_model->m_animExists)
    {
        _transforms.resize(1);
        _transforms[0] = glm::mat4(1.0);
        return;
    }

    unsigned int numBones = m_model->m_boneInfo.size();
    _transforms.resize(numBones);
    std::unordered_map<std::string, float> boneDeltas = m_revisionDiff->getBoneDeltas();


    ViewerUtilities::ReadNodeHierarchyDiff(m_model->m_boneMapping, _transforms, m_model->m_globalInverseTransform, _t, boneDeltas, m_model->m_rig, m_model->m_rig->m_rootBone, m_revisionDiff->getDiffRig(), identity);

}

