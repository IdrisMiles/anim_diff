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
        test->LoadModel("/home/george/projects/Collab/bin/GiantTeapot.obj");
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

    const aiScene *scene = _revision->m_model->m_scene;

    std::cout << scene << "\n";

    if(!scene)
    {
        std::cout<<"No valid AIScene\n";
    }
    else
    {
        if(scene->HasMeshes())
        {
            unsigned int indexOffset = 0;
            for(unsigned int i=0; i<scene->mNumMeshes; i++)
            {
                unsigned int numFaces = scene->mMeshes[i]->mNumFaces;
                for(unsigned int f=0; f<numFaces; f++)
                {
                    auto face = scene->mMeshes[i]->mFaces[f];
                    m_meshTris.push_back(glm::ivec3(face.mIndices[0]+indexOffset, face.mIndices[1]+indexOffset, face.mIndices[2]+indexOffset));
                }
                indexOffset += 3 * numFaces;

                unsigned int numVerts = scene->mMeshes[i]->mNumVertices;
                for(unsigned int v=0; v<numVerts; v++)
                {
                    auto vert = scene->mMeshes[i]->mVertices[v];
                    auto norm = scene->mMeshes[i]->mNormals[v];
                    m_meshVerts.push_back(glm::vec3(vert.x, vert.y, vert.z));
                    m_meshNorms.push_back(glm::vec3(norm.x, norm.y, norm.z));
                }
            }
        }
    }


    // m_shaderProg->bind();
    // glUniform3fv(m_colourLoc, 1, &m_colour[0]);

    // m_meshVAO.create();
    // m_meshVAO.bind();

    // m_meshIBO.create();
    // m_meshIBO.bind();
    // m_meshIBO.allocate(&m_meshTris[0], m_meshTris.size() * sizeof(int));
    // m_meshIBO.release();

    // // Setup our vertex buffer object.
    // m_meshVBO.create();
    // m_meshVBO.bind();
    // m_meshVBO.allocate(&m_meshVerts[0], m_meshVerts.size() * sizeof(glm::vec3));

    // glEnableVertexAttribArray( 0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    // m_meshVBO.release();


    // // Setup our normals buffer object.
    // m_meshNBO.create();
    // m_meshNBO.bind();
    // m_meshNBO.allocate(&m_meshNorms[0], m_meshNorms.size() * sizeof(glm::vec3));

    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(glm::vec3), 0);
    // m_meshNBO.release();


    // m_meshVAO.release();

    // m_shaderProg->release();

}
