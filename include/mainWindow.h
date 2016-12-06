#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Qt includes
#include <QMainWindow>
#include <QPushButton>
#include <memory>

class RepoController;
class RevisionViewer;
class DiffViewer;

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
    void UpdateRevisionTimers();
    void CompareRevisions();

private:
    Ui::MainWindow *ui;

    QTimer *m_animTimer;
    float m_time;
    float m_dt;

    std::shared_ptr<RevisionViewer> m_masterViewer;
    std::shared_ptr<RevisionViewer> m_branchViewer;
    std::shared_ptr<DiffViewer> m_diffViewer;

    std::unique_ptr<RepoController> m_repoController;
};

#endif // MAINWINDOW_H
