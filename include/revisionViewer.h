#ifndef REVISIONVIEWER_H
#define REVISIONVIEWER_H

#include <QTimer>

#include <vector>
#include <map>

#include "include/openglScene.h"
#include "include/revisionNode.h"
#include "include/viewerUtilities.h"


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


protected slots:
    /// @brief Method to update the bone animation. This takes advantage of Qt's Signals and Slots so that we can update the animation on a timer event to decouple it from the rest of the drawing.
    virtual void UpdateAnimation();


protected:
    /// @brief Method to do OpenGL drawing.
    virtual void paintGL() override;

    /// @brief Method to set up the shaders and all the attribute/uniform locations
    virtual void customInitGL() override;

    /// @brief Method to handle key input
    virtual void keyPressEvent(QKeyEvent *event) override;

    /// @brief Method to initialise all the relevant VAO and VBO's to draw an animated mesh.
    virtual void InitVAO();

    /// @brief Method to draw the model mesh.
    void DrawMesh();

    /// @brief Method to draw the rig mesh.
    void DrawRig();

    /// @brief Method to upload custom colours associated with rig joints to shader for model mesh and rig mesh.
    /// @param _rt : the type we are uploading colours to, either RIG or SKINNED.
    virtual void UploadBoneColoursToShader(RenderType _rt);

    /// @brief Method to upload all bone matrices to the shader for skinning. This is called every update.
    void UploadBonesToShader(const std::vector<glm::mat4> &_boneTransforms, RenderType _rt);

    /// @brief Method to transform bone hierarchy according to animation at time _t
    /// @param _t : animation time, this automatically gets looped if greater than animation duration.
    /// @param _transforms : bone transforms to be updated before sending to shader
    virtual void ComputeBoneTransform(const float _t, std::vector<glm::mat4> &_transforms);




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
    std::shared_ptr<Model> m_model;

};

#endif // REVISIONVIEWER_H
