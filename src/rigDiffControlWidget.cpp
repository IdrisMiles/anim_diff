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
    m_revisionDiff = _revisionDiff;
    m_layout = std::shared_ptr<QGridLayout>(new QGridLayout(this));

    int row = 0;

    // Master slider
    std::string masterName = "Master Slider";
    auto masterLabel = std::shared_ptr<QLabel>(new QLabel(QString(masterName.c_str()), this));
    auto masterSlider = std::shared_ptr<QSlider>(new QSlider(Qt::Horizontal, this));

    m_layout->addWidget(masterLabel.get(), row, 0, 1, 1);
    m_layout->addWidget(masterSlider.get(), row++, 1, 1, 1);

    m_masterJointController = std::make_tuple(masterName, masterLabel, masterSlider) ;
    std::get<2>(m_masterJointController)->setMaximum(100);
    std::get<2>(m_masterJointController)->setMinimum(0);
    std::get<2>(m_masterJointController)->setSliderPosition((int)(0*100.0f));

    connect(masterSlider.get(), SIGNAL(sliderMoved(int)), this, SLOT(UpdateMasterBoneDeltas()));


    // Dividing line between master and rig joints
    m_horizontalLine = std::shared_ptr<QFrame>(new QFrame(this));
    m_horizontalLine->setFrameShape(QFrame::HLine);
    m_horizontalLine->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(m_horizontalLine.get(), row++, 0, 1, 2);


    // Set up individual joint sliders
    auto boneDeltas = m_revisionDiff->getBoneDeltas();
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

        connect(jointSlider.get(), SIGNAL(sliderMoved(int)), this, SLOT(UpdateBoneDeltas()));

    }


    setLayout(m_layout.get());

}


void RigDiffControlWidget::UpdateBoneDeltas()
{
    std::unordered_map<std::string, float> newBoneDeltas;

    for(auto &&control : m_jointControls)
    {
        newBoneDeltas[std::get<0>(control)] = (float)std::get<2>(control)->sliderPosition()/100.0f;
    }


    m_revisionDiff->SetBoneDeltas(newBoneDeltas);
}

void RigDiffControlWidget::UpdateMasterBoneDeltas()
{

    for(auto &&control : m_jointControls)
    {
        std::get<2>(control)->setSliderPosition(std::get<2>(m_masterJointController)->sliderPosition());
    }



    std::unordered_map<std::string, float> newBoneDeltas;

    for(auto &&control : m_jointControls)
    {
        newBoneDeltas[std::get<0>(control)] = (float)std::get<2>(m_masterJointController)->sliderPosition()/100.0f;
    }


    m_revisionDiff->SetBoneDeltas(newBoneDeltas);
}
