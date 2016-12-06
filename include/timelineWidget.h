#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

// Qt includes
#include <QWidget>

namespace Ui {
class TimelineWidget;
}

class TimelineWidget : public QWidget
{
    Q_OBJECT

public:
    TimelineWidget(QWidget *parent = 0);
    ~TimelineWidget();

public slots:
    void updateDuration(double _duration);

signals:
    void newTime(double _time);
    void play();
    void pause();

private:

    Ui::TimelineWidget *ui;

};

#endif
