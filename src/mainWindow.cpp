#include "mainWindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGridLayout>

#include "repoController.h"
#include "revisionViewer.h"
#include "diffViewer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_repoController(new RepoController())
{
    ui->setupUi(this);

    //load up stylesheet
    QFile f("../QDarkStyleSheet/qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        this->setStyleSheet(ts.readAll());
    }

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

void MainWindow::LoadMasterRevision()
{
    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    // first time intialise
    if(!m_masterViewer)
    {
        m_masterViewer.reset(new RevisionViewer(this));
        ui->s_masterRevGB->layout()->addWidget(m_masterViewer.get());
        ui->s_masterRevBtn->setIcon(QIcon());
        ui->s_masterRevBtn->setText(QString("Load New Master"));
        //ui->s_masterRevBtn->hide();
    }

    auto node = m_repoController->loadMainNode(file.toStdString());
    m_masterViewer->LoadRevision(node);

    CompareRevisions();
}

void MainWindow::LoadBranchRevision()
{
    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    if(!m_branchViewer)
    {
        m_branchViewer.reset(new RevisionViewer(this));
        ui->s_branchRevGB->layout()->addWidget(m_branchViewer.get());
        ui->s_branchRevBtn->setIcon(QIcon());
        ui->s_branchRevBtn->setText(QString("Load New Branch"));
        ///ui->s_branchRevBtn->hide();
    }

    auto node = m_repoController->loadCompareNode(file.toStdString());
    m_branchViewer->LoadRevision(node);

    CompareRevisions();
}

void MainWindow::CompareRevisions()
{
    if(m_masterViewer && m_branchViewer)
    {
        m_diffViewer.reset(new DiffViewer(this));
        auto diff = m_repoController->getDiff();
        m_diffViewer->LoadDiff(diff);
        ui->s_diffRevGB->layout()->addWidget(m_diffViewer.get());
    }   
}

void MainWindow::UpdateRevisionTimers()
{
    m_time += m_dt;

    if(m_masterViewer) m_masterViewer->SetTime(m_time);
    if(m_branchViewer) m_branchViewer->SetTime(m_time);
    if(m_diffViewer) m_diffViewer->SetTime(m_time);
}
