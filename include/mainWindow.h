#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Qt includes
#include <QMainWindow>
#include <QPushButton>
#include <memory>

class RepoController;
class RevisionViewer;
class DiffViewer;
class RigDiffControlWidget;
class RevisionDiff;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void LoadMasterRevision();
    void LoadBranchRevision();
    void UpdateRevisionTimers(double _time);
    void CompareRevisions();
    void LoadDiffControls(std::shared_ptr<RevisionDiff> _diff);

private:
    Ui::MainWindow *ui;
    
    std::shared_ptr<RevisionViewer> m_masterViewer;
    std::shared_ptr<RevisionViewer> m_branchViewer;
    std::shared_ptr<DiffViewer> m_diffViewer;

    std::unique_ptr<RepoController> m_repoController;

    std::shared_ptr<RigDiffControlWidget> m_rigControls;
};

#endif // MAINWINDOW_H
