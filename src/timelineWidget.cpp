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
    connect(ui->button_playPause, &QPushButton::clicked, this, &TimelineWidget::playPause);

    // slider change
    // updates the timer to the side and other stuff
    connect(ui->slider_time, &QSlider::sliderMoved, this, &TimelineWidget::updateTime);

    ui->slider_time->setMinimum(0);
    ui->slider_time->setMaximum(0);
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
    // we times by 1000 to give us 1000 steps, because they are integer values
    ui->slider_time->setMaximum(1000 * (m_masterDuration > m_branchDuration ? m_masterDuration : m_branchDuration));
}

void TimelineWidget::updateTime(double _newTime)
{
    // cut down from 1000 to our actual time
    m_time = _newTime / 1000;

    updateClock();

    emit newTime(m_time);
}

void TimelineWidget::updateClock()
{
    ui->spin_time->setValue(m_time);
}

void TimelineWidget::updateSliderPos()
{
    // we times by 100 because thats waht we do on the slider
    ui->slider_time->setValue(m_time * 1000);
}

void TimelineWidget::incrementTime()
{
    m_time += m_dt;

    // this is for looping
    if(m_time > (m_masterDuration > m_branchDuration ? m_masterDuration : m_branchDuration)) m_time = 0.0;

    updateClock();
    updateSliderPos();

    // fire off a signal with the new time;
    emit newTime(m_time);
}

void TimelineWidget::playPause()
{
    if(m_timer->isActive())
    {
        m_timer->stop();
    }
    else
    {
        m_timer->start(1000*m_dt);
    }
}