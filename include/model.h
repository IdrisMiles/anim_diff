#ifndef MODEL_H
#define MODEL_H

// ASSIMP includes
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "modelRig.h"
#include "vertexBoneData.h"
#include "boneTransform.h"
#include "viewerUtilities.h"

#include <memory>

class QOpenGLShaderProgram;

class Model
{
public:
    Model();
    ~Model();

    void LoadModel(const std::string &_modelFile);
    void SetShader(QOpenGLShaderProgram **_shaderProg);

    void InitMesh(const aiScene *_scene);
    void InitRig(const aiScene *_scene);
    void SetRigVerts(aiNode *_pParentNode, aiNode *_pNode, const glm::mat4 &_parentTransform, const glm::mat4 &_thisTransform);
    void SetJointVert(const std::string _nodeName, const glm::mat4 &_transform, VertexBoneData &_vb);


    enum RenderType { SKINNED = 0, RIG = 1, NUMRENDERTYPES };
    float m_ticksPerSecond;
    float m_animationDuration;
    unsigned int m_numAnimations;
    unsigned int m_animationID;

    // Rig
    ModelRig m_rig;

    // Mesh info
    std::vector<glm::vec3> m_meshVerts;
    std::vector<glm::vec3> m_meshNorms;
    std::vector<glm::ivec3> m_meshTris;
    std::vector<VertexBoneData> m_meshBoneWeights;
    glm::vec3 m_colour;

    // Rig info
    std::vector<glm::vec3> m_rigVerts;
    std::vector<glm::vec3> m_rigNorms;
    std::vector<unsigned int> m_rigElements;
    std::vector<VertexBoneData> m_rigBoneWeights;
    std::vector<glm::vec3> m_rigJointColours;

    // Animation info
    bool m_animExists;
    std::vector<BoneTransformData> m_boneInfo;
    std::map<std::string, unsigned int> m_boneMapping;
    glm::mat4 m_globalInverseTransform;

    // OpenGL VAO and BO's
    QOpenGLVertexArrayObject m_meshVAO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshVBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshNBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshIBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshBWBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshCBO[NUMRENDERTYPES];

    // Shader locations
    GLuint m_vertAttrLoc[NUMRENDERTYPES];
    GLuint m_normAttrLoc[NUMRENDERTYPES];
    GLuint m_boneIDAttrLoc[NUMRENDERTYPES];
    GLuint m_boneWeightAttrLoc[NUMRENDERTYPES];
    GLuint m_boneUniformLoc[NUMRENDERTYPES];
    GLuint m_colourLoc[NUMRENDERTYPES];
    GLuint m_colourAttrLoc[NUMRENDERTYPES];
    GLuint m_projMatrixLoc[NUMRENDERTYPES];
    GLuint m_mvMatrixLoc[NUMRENDERTYPES];
    GLuint m_normalMatrixLoc[NUMRENDERTYPES];
    GLuint m_lightPosLoc[NUMRENDERTYPES];

    QOpenGLShaderProgram *m_shaderProg[NUMRENDERTYPES];

};

#endif // MODEL_H
