#ifndef MERGEDVIEWER_H
#define MERGEDVIEWER_H

#include <QTimer>

#include <vector>
#include <map>

#include "include/revisionViewer.h"
#include "include/diffViewer.h"
#include "include/revisionNode.h"
#include "include/revisionDiff.h"
#include "include/revisionMerge.h"
#include "include/viewerUtilities.h"


class MergeViewer : public RevisionViewer
{
public:
    /// @brief Constructor.
    /// @param parent : The parent widget to this widget.
    MergeViewer(QWidget *parent = nullptr);

    /// @brief Virtual destructor.
    virtual ~MergeViewer();

    /// @brief Method to load in a RevisionMerge for rendering.
    /// @param _merge : The revision merge data we want to draw.
    void LoadMerge(std::shared_ptr<RevisionMerge> _merge);



protected slots:
    /// @brief Method to update the bone animation. This takes advantage of Qt's Signals and Slots so that we can update the animation on a timer event to decouple it from the rest of the drawing.
    virtual void UpdateAnimation() override;

protected:
    /// @brief Method to do OpenGL drawing.
    virtual void paintGL() override;

    /// @brief Method to set up the shaders and all the attribute/uniform locations
    virtual void customInitGL() override;

    /// @brief Method to handle key input
    virtual void keyPressEvent(QKeyEvent *event) override;

    /// @brief Method to initialise all the relevant VAO and VBO's to draw an animated mesh and rig.
    virtual void UpdateVAOs();

    void ComputeBoneColours(const float _t);

    /// @brief Method to upload custom colours associated with rig joints to shader for model mesh and rig mesh.
    /// @param _rt : the type we are uploading colours to, either RIG or SKINNED.
    void UploadBoneColoursToShader(std::vector<glm::vec3> &_rigJointColour, RenderType _rt);


    /// @brief Method to transform bone hierarchy according to animation at time _t
    /// @param _t : animation time, this automatically gets looped if greater than animation duration.
    /// @param _transforms : bone transforms to be updated before sending to shader
    virtual void ComputeBoneTransform(const float _t, std::vector<glm::mat4> &_transforms);


    // Revision stuff
    std::shared_ptr<RevisionMerge> m_revisionMerge;
    std::shared_ptr<Model> m_modelMerge;
};

#endif // MERGEDVIEWER_H
