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
#include "include/revisionMerge.h"

class RigDiffControlWidget : public QGroupBox
{

    Q_OBJECT

public:    
    RigDiffControlWidget(QWidget *parent = nullptr);
    virtual ~RigDiffControlWidget();

    void LoadRig(std::shared_ptr<RevisionDiff> _revisionDiff);
    void LoadRig(std::shared_ptr<RevisionMerge> _revisionMerge);

public slots:
    void UpdateBoneDeltas();
    void UpdateMasterBoneDeltas();


private:
    typedef std::tuple<std::string, std::shared_ptr<QLabel>, std::shared_ptr<QSlider>> JointControlWidget;
    std::vector<JointControlWidget> m_jointControls;
    JointControlWidget m_masterJointController;
    std::shared_ptr<QGridLayout> m_layout;
    std::shared_ptr<QFrame> m_horizontalLine;

    std::shared_ptr<RevisionDiff> m_revisionDiff;
    std::shared_ptr<RevisionMerge> m_revisionMerge;
};

#endif // RIGDIFFCONTROLWIDGET_H
