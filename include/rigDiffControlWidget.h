#ifndef RIGDIFFCONTROLWIDGET_H
#define RIGDIFFCONTROLWIDGET_H

#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>

#include <memory>
#include <string>
#include <tuple>

#include "include/revisionDiff.h"

class RigDiffControlWidget : public QGroupBox
{
public:    
    RigDiffControlWidget(QWidget *parent = nullptr);
    ~RigDiffControlWidget();

    void LoadRig(std::shared_ptr<RevisionDiff> _revisionDiff);


private:
    typedef std::tuple<std::string, std::shared_ptr<QLabel>, std::shared_ptr<QSlider>> JointControl;
    std::vector<JointControl> m_jointControls;

    std::shared_ptr<QGridLayout> m_layout;
};

#endif // RIGDIFFCONTROLWIDGET_H
