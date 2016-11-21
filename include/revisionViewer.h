#ifndef REVISIONVIEWER_H
#define REVISIONVIEWER_H

#include <QTimer>

#include <vector>
#include <map>

#include "openglScene.h"
#include "revisionNode.h"
#include "viewerUtilities.h"
#include "renderModel.h"
#include "modelRig.h"

#include <assimp/matrix4x4.h>



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

    /// @brief Method to set the current time of the animation, to be used externally.
    /// @param _t : The time we are settting.
    void SetTime(const float _t);


private slots:
    /// @brief Method to update the bone animation. This takes advantage of Qt's Signals and Slots so that we can update the animation on a timer event to decouple it from the rest of the drawing.
    virtual void UpdateAnimation();


protected:
    /// @brief Method to do OpenGL drawing.
    void paintGL() override;
    void customInitGL() override;
    void keyPressEvent(QKeyEvent *event) override;


private:
    /// @brief Method to initialise all the relevant VAO and VBO's to draw an animated mesh.
    void InitVAO();
    //void InitMesh();
    //void InitRig();
    //void SetRigVerts(aiNode *_pParentNode, aiNode *_pNode, const aiMatrix4x4 &_parentTransform, const aiMatrix4x4 &_thisTransform);
    //void SetJointVert(const std::string _nodeName, const aiMatrix4x4 &_transform, VertexBoneData &_vb);

    void DrawMesh();
    void DrawRig();

    /// @brief Method to upload all bone matrices to the shader for skinning. This is called every update.
    void UploadBonesToShader(const float _t, RenderType _rt);

    /// @brief Method to transform bone hierarchy according to animation at time _t
    /// @param _t : animation time, this automatically gets looped if greater than animation duration.
    /// @param _transforms : bone transforms to be updated before sending to shader
    void BoneTransform(const float _t, std::vector<glm::mat4> &_transforms);




    bool m_revisionLoaded;
    bool m_initGL;
    bool m_waitingForInitGL;

    bool m_wireframe;
    bool m_drawMesh;
    bool m_playAnim;
    float m_dt;
    float m_t;

    QTimer * m_animTimer;
    QTimer * m_drawTimer;

    // Revision stuff
    std::shared_ptr<RevisionNode> m_revision;
    Model m_model;

};

#endif // REVISIONVIEWER_H
