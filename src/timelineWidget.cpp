#include "timelineWidget.h"
#include "ui_timeline.h"
#include <QTimer>

TimelineWidget::TimelineWidget(QWidget *parent)
:
    QWidget(parent),
    ui(new Ui::TimelineWidget()),
    m_timer(new QTimer(this)),
    m_time(0.0f),
    m_dt(0.016f),
    m_masterDuration(0.0f),
    m_branchDuration(0.0f)
{
    ui->setupUi(this);

    // connect timer
    connect(m_timer.get(), &QTimer::timeout, this, &TimelineWidget::incrementTime);

    // connect buttons. could of used lambda's instead of private methods but oh well 
    connect(ui->button_play, &QPushButton::clicked, this, &TimelineWidget::play);
    connect(ui->button_pause, &QPushButton::clicked, this, &TimelineWidget::pause);

    // slider change
    // updates the timer to the side and other stuff
    connect(ui->slider_time, &QSlider::sliderMoved, this, &TimelineWidget::updateTime);

    ui->slider_time->setMinimum(0.0);
    ui->slider_time->setMaximum(0.0);
}

TimelineWidget::~TimelineWidget()
{

}

void TimelineWidget::updateMasterDuration(float _duration)
{
    m_masterDuration = _duration;
    updateDuration();
}

void TimelineWidget::updateBranchDuration(float _duration)
{
    m_branchDuration = _duration;
    updateDuration();
}

void TimelineWidget::updateDuration()
{
    ui->slider_time->setMaximum(m_masterDuration > m_branchDuration ? m_masterDuration : m_branchDuration);
}

void TimelineWidget::updateTime(double _newTime)
{
    m_time = _newTime;

    updateGUI();

    emit newTime(m_time);
}

void TimelineWidget::updateGUI()
{
    ui->spin_time->setValue(m_time);

    //update slider;
}

void TimelineWidget::incrementTime()
{
    m_time += m_dt;

    updateGUI();
    // fire off a signal with the new time;
    emit newTime(m_time);
}

void TimelineWidget::play()
{
    //start timer from existing point
    m_timer->start(1000*m_dt);
}

void TimelineWidget::pause()
{
    // stop timer
    m_timer->stop();
}