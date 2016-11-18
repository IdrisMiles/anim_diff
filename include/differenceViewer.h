#ifndef DIFFERENCEVIEWER_H
#define DIFFERENCEVIEWER_H

#include "openglScene.h"
#include "revisionViewer.h"
#include "revisionDiff.h"

class DifferenceViewer : public OpenGLScene
{

public:


    /// @brief Constructor.
    /// @param parent : The parent widget to this widget.
    DifferenceViewer(QWidget *parent);

    /// @brief Virtual destructor.
    virtual ~DifferenceViewer();

    /// @brief Method to load in a RevisionNode for rendering.
    /// @param _revision : The revision we want to draw.
    void LoadDifference(std::shared_ptr<RevisionDiff> _diff);


    void LoadRevision(std::shared_ptr<RevisionNode> _revision);


    std::shared_ptr<RevisionViewer> GetRevisionView(const int &_i);




private slots:
    /// @brief Method to update the bone animation. This takes advantage of Qt's Signals and Slots so that we can update the animation on a timer event to decouple it from the rest of the drawing.
    virtual void UpdateAnimation() override;

protected:

private:

    // Difference stuff
    std::shared_ptr<RevisionDiff> m_revisionDiff;

    // New revision stuff
    std::shared_ptr<RevisionNode> m_newRevision;
    const aiScene *m_newScene;


};

#endif //DIFFERENCEVIEWER_H
