#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "priorityscheduling.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_runButton_clicked();

    void on_pauseButton_clicked();

private:
    Ui::MainWindow *ui;
    int CPUnum;
    int num;
    PriorityScheduling* w;
};

#endif // MAINWINDOW_H
