#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Qt includes
#include <QMainWindow>
#include <QPushButton>
#include <memory>

#include "revisionViewer.h"

class RepoController;

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
    void LoadRevision();
    void LoadMasterRevision();
    void LoadBranchRevision();
    void UpdateRevisionTimers();

private:
    Ui::MainWindow *ui;

    QTimer *m_animTimer;
    float m_time;
    float m_dt;

    std::vector<RevisionViewer*> m_revisionViewers;
    std::unique_ptr<RepoController> m_repoController;
};

#endif // MAINWINDOW_H
