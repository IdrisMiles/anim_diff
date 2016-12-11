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
    connect(ui->s_compareBtn, &QPushButton::clicked, this, &MainWindow::CompareRevisions);

    // link the timer stuff
    connect(ui->timeline, &TimelineWidget::newTime, this, &MainWindow::UpdateRevisionTimers);    
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

    // update timeline
    ui->timeline->updateMasterDuration(node->m_model->m_animationDuration);

    //disable comparison
    if(m_diffViewer)
    {
        m_diffViewer->hide();
        ui->s_compareBtn->show();
    }
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

    // update timeline;
    ui->timeline->updateBranchDuration(node->m_model->m_animationDuration);

    //disable comparison and show compare button
    if(m_diffViewer)
    {
        m_diffViewer->hide();
        ui->s_compareBtn->show();
    }
}

void MainWindow::CompareRevisions()
{
    if(m_masterViewer && m_branchViewer)
    {
        m_diffViewer.reset(new DiffViewer(this));
        auto diff = m_repoController->getDiff();
        m_diffViewer->LoadDiff(diff);
        ui->s_diffRevGB->layout()->addWidget(m_diffViewer.get());

        // get rid of button
        ui->s_compareBtn->hide();
        m_diffViewer->show();
    }   
}

void MainWindow::UpdateRevisionTimers(double _time)
{
    if(m_masterViewer) m_masterViewer->SetTime(_time);
    if(m_branchViewer) m_branchViewer->SetTime(_time);
    if(m_diffViewer) m_diffViewer->SetTime(_time);
}
