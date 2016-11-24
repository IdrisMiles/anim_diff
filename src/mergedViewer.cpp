#include "mergedViewer.h"
#include <QKeyEvent>



MergedViewer::MergedViewer(QWidget *parent) : OpenGLScene(parent)
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
    connect(m_animTimer, &QTimer::timeout, this, &MergedViewer::UpdateAnimation);

    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(update()));
}


MergedViewer::~MergedViewer()
{
    delete m_animTimer;
    delete m_drawTimer;

    for(unsigned int i=0; i<NUMRENDERTYPES; i++)
    {
        m_model->m_meshVBO[i].destroy();
        m_model->m_meshNBO[i].destroy();
        m_model->m_meshIBO[i].destroy();
        m_model->m_meshBWBO[i].destroy();
        m_model->m_meshVAO[i].destroy();

        m_model->m_shaderProg[i]->destroyed();
        delete m_model->m_shaderProg[i];
    }

}

void MergedViewer::LoadMerge(std::shared_ptr<RevisionMerge> _diff)
{

}

void MergedViewer::UpdateAnimation()
{

}


void MergedViewer::paintGL()
{
    if(m_waitingForInitGL)
    {
        LoadMerge(m_revision);
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

void MergedViewer::customInitGL()
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

void MergedViewer::InitVAO()
{

}

void MergedViewer::DrawMesh()
{

}

void MergedViewer::DrawRig()
{

}

void MergedViewer::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        m_wireframe = !m_wireframe;
    }
}
