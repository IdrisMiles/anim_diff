#include "include/rigDiffControlWidget.h"
#include "include/revisionDiff.h"

RigDiffControlWidget::RigDiffControlWidget(QWidget *parent) : QGroupBox(parent)
{
}

RigDiffControlWidget::~RigDiffControlWidget()
{

}


void RigDiffControlWidget::LoadRig(std::shared_ptr<RevisionDiff> _revisionDiff)
{
    m_layout = std::shared_ptr<QGridLayout>(new QGridLayout(this));

    // iterate through each joint of bone and add to our list
    int row = 0;
    auto boneDeltas = _revisionDiff->getBoneDeltas();//->getMasterNode()->m_model->m_rig->m_boneAnims;
    for(auto bone : boneDeltas)
    {
        std::string jointName = bone.first;
        auto jointLabel = std::shared_ptr<QLabel>(new QLabel(QString(jointName.c_str()), this));
        auto jointSlider = std::shared_ptr<QSlider>(new QSlider(Qt::Horizontal, this));

        m_layout->addWidget(jointLabel.get(), row, 0, 1, 1);
        m_layout->addWidget(jointSlider.get(), row++, 1, 1, 1);

        m_jointControls.push_back( std::make_tuple(jointName, jointLabel, jointSlider) );
        std::get<2>(m_jointControls.back())->setMaximum(100);
        std::get<2>(m_jointControls.back())->setMinimum(0);
        std::get<2>(m_jointControls.back())->setSliderPosition((int)(bone.second*100.0f));
    }
    setLayout(m_layout.get());


    // Connect up all the sliders
}
