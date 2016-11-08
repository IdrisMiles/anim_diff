#ifndef REVISIONVIEWER_H
#define REVISIONVIEWER_H

#include <QTimer>

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

public slots:
    virtual void UpdateAnimation();


private:
    void InitVAO();
    void InitMesh();
    void InitAnimation();

    /// @brief Method to upload all bone matrices to the shader for skinning. This is called every update.
    void UploadBonesToShader(const float _t);

    /// @brief Method to transform bone hierarchy according to animation at time _t
    /// @param float _t : animation time, this automatically gets looped if greater than animation duration.
    /// @param std::vector<aiMatrix4x4> &_transforms : bone transforms to be updated before sending to shader
    void BoneTransform(const float _t, std::vector<glm::mat4> &_transforms);

    /// @brief Method that recursively reads the animation node hierarchy and accumulates the relevant transforms for the current animation time.
    /// @param float _animationTime : Animation time.
    /// @param const aiAnimation *_pAnimation : The animation we want to use.
    /// @param const aiNode* _pNode : The transform node we want to find a corresponding aiNodeAnim for to read its animation.
    /// @param const aiMatrix4x4& _parentTransform : The accumulated parent transform, so as we traverse the hierarchy each bone has the correcct global transformation.
    void ReadNodeHierarchy(const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform);

    /// @brief Method to find the animation node with the name matching NodeName.
    /// @param const aiAnimation *_pAnimation : The animation we want to use.
    /// @param const std::string _nodeName : The name of the animated node in the animation we are trying to find.
    /// @return const aiNodeAnim* : returns the aiNodeAnim with the same name as _nodeName in _pAnimation if found, else returns NULL.
    const aiNodeAnim* FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName);

    /// @brief Method to interpolate between two frame of rotation animation.
    /// @param aiQuaternion& _out : Quaternion to hold the interpolated rotation animation for the current animation time in the current animation.
    /// @param float _animationTime : The current animation time.
    /// @param const aiNodeAnim* _pNodeAnim : The animated node we are getting the rotation from.
    void CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of position animation.
    /// @param aiVector3D& _out : Vector to hold the interpolated position animation for the current animation time in the current animation.
    /// @param float _animationTime : The current animation time.
    /// @param const aiNodeAnim* _pNodeAnim : The animated node we are getting the position from.
    void CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of scaling animation.
    /// @param aiVector3D& _out : Vector to hold the interpolated scaling animation for the current animation time in the current animation.
    /// @param float _animationTime : The current animation time.
    /// @param const aiNodeAnim* _pNodeAnim : The animated node we are getting the scaling from.
    void CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the rotation animation of this aiNodeAnim.
    /// @param float _animationTime : The current animation time.
    /// @param const aiNodeAnim* _pNodeAnim : The animated node we are getting the keyframe for rotation animation from.
    uint FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the position animation of this aiNodeAnim.
    /// @param float _animationTime : The current animation time.
    /// @param const aiNodeAnim* _pNodeAnim : The animated node we are getting the keyframe for position animation from.
    uint FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the scaling animation of this aiNodeAnim.
    /// @param float _animationTime : The current animation time.
    /// @param const aiNodeAnim* _pNodeAnim : The animated node we are getting the keyframe for scaling animation from.
    uint FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);


    bool m_wireframe;
    bool m_drawMesh;
    bool m_playAnim;
    bool m_animExists;
    float m_dt;
    QTimer * m_animTimer;
    QTimer * m_drawTimer;


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

    // Animation info
    std::vector<BoneInfo> m_boneInfo;
    std::map<std::string, unsigned int> m_boneMapping;
    std::map<std::string, unsigned int> m_boneAnimChannelMapping;
    float m_ticksPerSecond;
    float m_animationDuration;
    aiMatrix4x4 m_globalInverseTransform;

    const aiScene *m_scene;
    std::shared_ptr<RevisionNode> m_revision;


    // Shader locations
    GLuint m_vertAttrLoc;
    GLuint m_normAttrLoc;
    GLuint m_boneIDAttrLoc;
    GLuint m_boneWeightAttrLoc;
    GLuint m_boneUniformLoc;
    GLuint m_colourLoc;
    glm::vec3 m_colour;




};

#endif // REVISIONVIEWER_H
