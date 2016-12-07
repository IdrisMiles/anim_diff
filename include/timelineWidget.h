#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

// Qt includes
#include <QWidget>
#include <memory>

namespace Ui {
class TimelineWidget;
}

class QTimer;

class TimelineWidget : public QWidget
{
    Q_OBJECT

public:
    TimelineWidget(QWidget *parent = 0);
    ~TimelineWidget();

    void updateMasterDuration(float _duration);
    void updateBranchDuration(float _duration);

signals:
    void newTime(double _time);

private:
    void playPause();
    void incrementTime();
    void updateClock();
    void updateSliderPos();
    void updateTime(double _newTime);
    void updateDuration();

private:

    Ui::TimelineWidget *ui;

    std::unique_ptr<QTimer> m_timer;
    float m_time;
    float m_dt;
    float m_masterDuration;
    float m_branchDuration;
};

#endif
