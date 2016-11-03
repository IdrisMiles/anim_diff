#ifndef REVISIONVIEWER_H
#define REVISIONVIEWER_H

#include <vector>
#include <map>

#include "include/openglscene.h"
#include "include/revision.h"

#include <assimp/matrix4x4.h>

struct VertexBoneData
{
    unsigned int boneID[4];
    float boneWeight[4];
};

struct BoneInfo
{
    aiMatrix4x4 boneOffset;
    aiMatrix4x4 finalTransform;
};

class RevisionViewer : public OpenGLScene
{
public:
    RevisionViewer(QWidget *parent);
    virtual ~RevisionViewer();

    void LoadRevision(std::shared_ptr<RevisionNode> _revision);

protected:
    void paintGL() override;
    void InitVAO();
    void InitMesh();
    void InitAnimation();
    aiMatrix4x4 BoneTransform(float _t, std::vector<aiMatrix4x4> &_transforms);
    void ReadNodeHierarchy(float _animationTime, const aiNode* _pNode, const aiMatrix4x4& _parentTransform);

private:

    bool m_wireframe;
    bool m_drawMesh;


    // OpenGL VAO and BO's
    QOpenGLVertexArrayObject m_meshVAO;
    QOpenGLBuffer m_meshVBO;
    QOpenGLBuffer m_meshNBO;
    QOpenGLBuffer m_meshIBO;
    QOpenGLBuffer m_meshBWBO;

    // Mesh info
    std::vector<glm::vec3> m_meshVerts;
    std::vector<glm::vec3> m_meshNorms;
    std::vector<glm::ivec3> m_meshTris;
    std::vector<VertexBoneData> m_meshBoneWeights;
    std::vector<glm::mat4> m_boneOffsetTransforms;
    std::vector<BoneInfo> m_boneInfo;
    std::map<std::string, unsigned int> m_boneMapping;
    float m_ticksPerSecond;
    float m_animationDuration;
    const aiScene *m_scene;
    aiMatrix4x4 m_globalInverseTransform;


    // Shader locations
    GLuint m_vertAttrLoc;
    GLuint m_normAttrLoc;
    GLuint m_boneIDAttrLoc;
    GLuint m_boneWeightAttrLoc;
    GLuint m_colourLoc;
    glm::vec3 m_colour;




};

#endif // REVISIONVIEWER_H
