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


/// @class RevisionViewer is a class designed to specifically draw a RevisionNode, it inherits from OpenGLSene so that it has the relevant components for drawing with OpenGL. It is intended to decouple the front end visualisation with the backend version control.
class RevisionViewer : public OpenGLScene
{

public:
    /// @brief Constructor.
    /// @param parent : The parent widget to this widget.
    RevisionViewer(QWidget *parent);

    /// @brief Virtual destructor.
    virtual ~RevisionViewer();

    /// @brief Method to load in a RevisionNode for rendering.
    /// @param _revision : The revision we want to draw.
    void LoadRevision(std::shared_ptr<RevisionNode> _revision);

protected:
    /// @brief Method to do OpenGL drawing.
    void paintGL() override;

private slots:
    /// @brief Method to update the bone animation. This takes advantage of Qt's Signals and Slots so that we can update the animation on a timer event to decouple it from the rest of the drawing.
    virtual void UpdateAnimation();


private:
    /// @brief Method to initialise all the relevant VAO and VBO's to draw an animated mesh.
    void InitVAO();

    void InitMesh();
    void InitRig();
    void GetRigTransforms(const aiNode* _pNode, const aiMatrix4x4 &_parentTransform, unsigned int &_currentID, const unsigned int _prevID);
    void InitAnimation();
    void DrawMesh();
    void DrawRig();

    /// @brief Method to upload all bone matrices to the shader for skinning. This is called every update.
    void UploadBonesToShader(const float _t);

    /// @brief Method to transform bone hierarchy according to animation at time _t
    /// @param _t : animation time, this automatically gets looped if greater than animation duration.
    /// @param _transforms : bone transforms to be updated before sending to shader
    void BoneTransform(const float _t, std::vector<glm::mat4> &_transforms);

    /// @brief Method that recursively reads the animation node hierarchy and accumulates the relevant transforms for the current animation time.
    /// @param _animationTime : Animation time.
    /// @param _pAnimation : The animation we want to use.
    /// @param _pNode : The transform node we want to find a corresponding aiNodeAnim for to read its animation.
    /// @param _parentTransform : The accumulated parent transform, so as we traverse the hierarchy each bone has the correcct global transformation.
    void ReadNodeHierarchy(const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform);

    /// @brief Method to find the animation node with the name matching NodeName.
    /// @param _pAnimation : The animation we want to use.
    /// @param _nodeName : The name of the animated node in the animation we are trying to find.
    /// @return const aiNodeAnim* : returns the aiNodeAnim with the same name as _nodeName in _pAnimation if found, else returns NULL.
    const aiNodeAnim* FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName);

    /// @brief Method to interpolate between two frame of rotation animation.
    /// @param _out : Quaternion to hold the interpolated rotation animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the rotation from.
    void CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of position animation.
    /// @param _out : Vector to hold the interpolated position animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the position from.
    void CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of scaling animation.
    /// @param _out : Vector to hold the interpolated scaling animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the scaling from.
    void CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the rotation animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for rotation animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    uint FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the position animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for position animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    uint FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the scaling animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for scaling animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    uint FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);


    bool m_wireframe;
    bool m_drawMesh;
    bool m_playAnim;
    bool m_animExists;
    float m_dt;
    QTimer * m_animTimer;
    QTimer * m_drawTimer;

    enum RenderType { SKINNED = 0, RIG = 1, NUMRENDERTYPES };

    // OpenGL VAO and BO's
    QOpenGLVertexArrayObject m_meshVAO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshVBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshNBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshIBO[NUMRENDERTYPES];
    QOpenGLBuffer m_meshBWBO[NUMRENDERTYPES];

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

    // Animation info
    std::vector<BoneInfo> m_boneInfo;
    std::map<std::string, unsigned int> m_boneMapping;
    float m_ticksPerSecond;
    float m_animationDuration;
    aiMatrix4x4 m_globalInverseTransform;

    // Revision stuff
    std::shared_ptr<RevisionNode> m_revision;
    const aiScene *m_scene;

    // Shader locations
    GLuint m_vertAttrLoc;
    GLuint m_normAttrLoc;
    GLuint m_boneIDAttrLoc;
    GLuint m_boneWeightAttrLoc;
    GLuint m_boneUniformLoc;
    GLuint m_colourLoc;




};

#endif // REVISIONVIEWER_H
