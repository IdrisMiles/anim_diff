#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Qt includes
#include <QMainWindow>

#include "revisionviewer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    RevisionViewer *m_scene;
};

#endif // MAINWINDOW_H
