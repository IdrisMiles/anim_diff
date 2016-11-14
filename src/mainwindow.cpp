#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_loadRevisionButton = new QPushButton("Load Revision", this);
    ui->gridLayout->addWidget(m_loadRevisionButton, 2, 1, 1, 1);
    connect(m_loadRevisionButton, SIGNAL(clicked(bool)), this, SLOT(LoadRevision()));

    m_time = 0.0f;
    m_dt = 0.016f;
    m_animTimer = new QTimer(this);
    connect(m_animTimer, SIGNAL(timeout()), this, SLOT(UpdateRevisionTimers()));
    m_animTimer->start(1000*m_dt);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadRevision()
{

    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    m_revisions.push_back(new RevisionViewer(this));
    std::shared_ptr<RevisionNode> test(new RevisionNode());

    test->LoadModel(file.toStdString());
    m_revisions.back()->LoadRevision(test);

    ui->gridLayout->addWidget(m_revisions.back(), 1, m_revisions.size(), 1, 1);
}

void MainWindow::UpdateRevisionTimers()
{
    m_time += m_dt;
    for(auto rev : m_revisions)
    {
        rev->SetTime(m_time);
    }
}
