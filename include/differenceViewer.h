#ifndef DIFFERENCEVIEWER_H
#define DIFFERENCEVIEWER_H

#include "include/openglscene.h"
#include "include/revisionviewer.h"

class DifferenceViewer : public OpenGLScene
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

protected:
    /// @brief Method to do OpenGL drawing.
    void paintGL() override;
    void customInitGL() override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:


private:

};

#endif //DIFFERENCEVIEWER_H
