#ifndef REVISIONVIEWER_H
#define REVISIONVIEWER_H

#include "include/openglscene.h"
#include "include/revision.h"

class RevisionViewer : public OpenGLScene
{
public:
    RevisionViewer(QWidget *parent);
    virtual ~RevisionViewer();

    void LoadRevision(std::shared_ptr<RevisionNode> _revision);

protected:

    void paintGL() override;

private:

    bool m_wireframe;
    bool m_drawMesh;

    QOpenGLVertexArrayObject m_meshVAO;
    QOpenGLBuffer m_meshVBO;
    QOpenGLBuffer m_meshNBO;
    QOpenGLBuffer m_meshIBO;
    QOpenGLBuffer m_meshBWBO;

    std::vector<glm::vec3> m_meshVerts;
    std::vector<glm::vec3> m_meshNorms;
    std::vector<glm::ivec3> m_meshTris;
    std::vector<glm::vec4> m_meshBoneWeights;

    int m_colourLoc;
    glm::vec3 m_colour;




};

#endif // REVISIONVIEWER_H
