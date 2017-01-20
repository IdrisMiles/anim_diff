#include "mainWindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QGridLayout>

#include "repoController.h"
#include "revisionViewer.h"
#include "diffViewer.h"
#include "mergeViewer.h"
#include "rigDiffControlWidget.h"

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

    ui->s_rigDiffControlsGB->hide();

    connect(ui->s_masterRevBtn, SIGNAL(clicked(bool)), this, SLOT(LoadMasterRevision()));
    connect(ui->s_branchRevBtn, SIGNAL(clicked(bool)), this, SLOT(LoadBranchRevision()));
    connect(ui->s_parentRevBtn, SIGNAL(clicked(bool)), this, SLOT(LoadParentRevision()));
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
    m_masterViewer->SetMeshColour(glm::vec3(0.6f,0.6f,0.6f));

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
    m_branchViewer->SetMeshColour(glm::vec3(0.6f,0.6f,0.6f));

    // update timeline;
    ui->timeline->updateBranchDuration(node->m_model->m_animationDuration);

    //disable comparison and show compare button
    if(m_diffViewer)
    {
        m_diffViewer->hide();
        ui->s_compareBtn->show();
    }
}

void MainWindow::LoadParentRevision()
{
    QString file = QFileDialog::getOpenFileName(this,QString("Open File"), QString("./"), QString("3D files (*.*)"));

    if (file.isNull())
    {
        return;
    }

    if(!m_parentViewer)
    {
        m_parentViewer.reset(new RevisionViewer(this));
        ui->s_diffRevGB->layout()->addWidget(m_parentViewer.get());
        ui->s_parentRevBtn->setIcon(QIcon());
        ui->s_parentRevBtn->setText(QString("Load New Parent"));
        ///ui->s_branchRevBtn->hide();
    }

    auto node = m_repoController->loadParentNode(file.toStdString());
    m_parentViewer->LoadRevision(node);
    m_parentViewer->SetMeshColour(glm::vec3(0.6f,0.6f,0.6f));

    // TODO add parent time to timer
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
        if(m_parentViewer)
        {
            m_mergeViewer.reset(new MergeViewer(this));
            auto merge = m_repoController->getMerge();
            m_mergeViewer->LoadMerge(merge);
            ui->s_diffRevGB->layout()->addWidget(m_mergeViewer.get());

            // get rid of button
            ui->s_compareBtn->hide();
            m_parentViewer->hide();
            m_mergeViewer->show();

            LoadDiffControls(merge);
            m_masterViewer->SetMeshColour(glm::vec3(0.4f,0.8f,0.4f));
            m_branchViewer->SetMeshColour(glm::vec3(0.4f,0.4f,0.8f));
        }
        else
        {
            m_diffViewer.reset(new DiffViewer(this));
            auto diff = m_repoController->getDiff();
            m_diffViewer->LoadDiff(diff);
            ui->s_diffRevGB->layout()->addWidget(m_diffViewer.get());

            // get rid of button
            ui->s_compareBtn->hide();
            m_diffViewer->show();
            ui->s_parentRevBtn->hide();

            LoadDiffControls(diff);
        }
    }   
}

void MainWindow::LoadDiffControls(std::shared_ptr<RevisionDiff> _diff)
{
    if(_diff != nullptr)
    {
        m_rigControls.reset(new RigDiffControlWidget(this));
        m_rigControls->LoadRig(_diff);
        ui->s_rigDiffControlsGB->layout()->addWidget(m_rigControls.get());
        ui->s_rigDiffControlsGB->show();
    }
}

void MainWindow::LoadDiffControls(std::shared_ptr<RevisionMerge> _merge)
{
    if(_merge != nullptr)
    {
        m_rigControls.reset(new RigDiffControlWidget(this));
        m_rigControls->LoadRig(_merge);
        ui->s_rigDiffControlsGB->layout()->addWidget(m_rigControls.get());
        ui->s_rigDiffControlsGB->show();
    }
}

void MainWindow::UpdateRevisionTimers(double _time)
{
    if(m_masterViewer) m_masterViewer->SetTime(_time);
    if(m_branchViewer) m_branchViewer->SetTime(_time);
    if(m_diffViewer) m_diffViewer->SetTime(_time);
    if(m_mergeViewer) m_mergeViewer->SetTime(_time);
    if(m_parentViewer) m_parentViewer->SetTime(_time);
}
