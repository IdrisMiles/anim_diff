#include "timelineWidget.h"
#include "ui_timeline.h"

TimelineWidget::TimelineWidget(QWidget *parent)
:
    QWidget(parent),
    ui(new Ui::TimelineWidget())
{
    ui->setupUi(this);

    //connections

    // slider change
    // updates the timer to the side and other stuff

    //need to update the time range and stuff
}

TimelineWidget::~TimelineWidget()
{
}

void TimelineWidget::updateDuration(double _duration)
{

}