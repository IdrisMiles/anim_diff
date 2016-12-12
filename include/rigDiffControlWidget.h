#ifndef RIGDIFFCONTROLWIDGET_H
#define RIGDIFFCONTROLWIDGET_H

#include <QGroupBox>
#include <QSlider>
#include <QLabel>

#include <memory>
#include <string>
#include <tuple>

#include "include/revisionDiff.h"

class RigDiffControlWidget : QGroupBox
{
public:    
    RigDiffControlWidget(QWidget *parent = nullptr);

    void LoadRig(std::shared_ptr<RevisionDiff>);

    typedef std::tuple<std::string, std::shared_ptr<QLabel>, std::shared_ptr<QSlider>> JointControl;
    std::vector<JointControl> m_jointControls;
};

#endif // RIGDIFFCONTROLWIDGET_H
