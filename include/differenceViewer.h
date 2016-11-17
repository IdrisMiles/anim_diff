#ifndef DIFFERENCEVIEWER_H
#define DIFFERENCEVIEWER_H

#include "include/openglScene.h"
#include "include/revisionViewer.h"

class DifferenceViewer : public RevisionViewer
{

public:

    enum RenderType { SKINNED = 0, RIG = 1, NUMRENDERTYPES };

    /// @brief Constructor.
    /// @param parent : The parent widget to this widget.
    DifferenceViewer(QWidget *parent);

    /// @brief Virtual destructor.
    virtual ~DifferenceViewer();

    /// @brief Method to load in a RevisionNode for rendering.
    /// @param _revision : The revision we want to draw.
    void LoadRevision(std::shared_ptr<RevisionNode> _revision);

    std::shared_ptr<RevisionViewer> GetRevisionView(const int &_i);


private slots:
    /// @brief Method to update the bone animation. This takes advantage of Qt's Signals and Slots so that we can update the animation on a timer event to decouple it from the rest of the drawing.
    virtual void UpdateAnimation() override;

protected:

private:
    // New revision stuff
    std::shared_ptr<RevisionNode> m_newRevision;
    const aiScene *m_newScene;


};

#endif //DIFFERENCEVIEWER_H
