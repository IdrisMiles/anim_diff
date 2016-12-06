#include "mainWindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGridLayout>

#include "repoController.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_repoController(new RepoController())
{
    ui->setupUi(this);

    connect(ui->s_masterRevBtn, SIGNAL(clicked(bool)), this, SLOT(LoadMasterRevision()));
    connect(ui->s_branchRevBtn, SIGNAL(clicked(bool)), this, SLOT(LoadBranchRevision()));

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
    // this is all a hack and will be changed with the new UI
    if(m_revisionViewers.size() >= 2) return;

    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    m_revisionViewers.push_back(new RevisionViewer(this));

    if(m_revisionViewers.empty())
    {
        auto node = m_repoController->loadMainNode(file.toStdString());
        m_revisionViewers.back()->LoadRevision(node);
    }
    else
    {
        auto node = m_repoController->loadCompareNode(file.toStdString());
        m_revisionViewers.back()->LoadRevision(node);
    }

    ui->gridLayout->addWidget(m_revisionViewers.back(), 1, m_revisionViewers.size(), 1, 1);
}

void MainWindow::LoadMasterRevision()
{
    // this is all a hack and will be changed with the new UI
    if(m_revisionViewers.size() >= 2) return;

    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    m_revisionViewers.push_back(new RevisionViewer(this));

    if(m_revisionViewers.empty())
    {
        auto node = m_repoController->loadMainNode(file.toStdString());
        m_revisionViewers.back()->LoadRevision(node);
    }
    else
    {
        auto node = m_repoController->loadCompareNode(file.toStdString());
        m_revisionViewers.back()->LoadRevision(node);
    }

    ui->s_masterRevGB->layout()->addWidget(m_revisionViewers.back());
    ui->s_masterRevBtn->hide();
    //ui->gridLayout->addWidget(m_revisionViewers.back(), 1, m_revisionViewers.size(), 1, 1);
}

void MainWindow::LoadBranchRevision()
{
    // this is all a hack and will be changed with the new UI
    if(m_revisionViewers.size() >= 2) return;

    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    m_revisionViewers.push_back(new RevisionViewer(this));

    if(m_revisionViewers.empty())
    {
        auto node = m_repoController->loadMainNode(file.toStdString());
        m_revisionViewers.back()->LoadRevision(node);
    }
    else
    {
        auto node = m_repoController->loadCompareNode(file.toStdString());
        m_revisionViewers.back()->LoadRevision(node);
    }

    ui->s_branchRevGB->layout()->addWidget(m_revisionViewers.back());
    ui->s_branchRevBtn->hide();
    //ui->gridLayout->addWidget(m_revisionViewers.back(), 1, m_revisionViewers.size(), 1, 1);
}

void MainWindow::UpdateRevisionTimers()
{
    m_time += m_dt;
    for(auto rev : m_revisionViewers)
    {
        rev->SetTime(m_time);
    }
}
