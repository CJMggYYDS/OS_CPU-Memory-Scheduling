#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("优先级调度-参数设置");
    this->setFixedSize(this->width(), this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_runButton_clicked()
{
    CPUnum=ui->CPULine->text().toInt();
    num=ui->numberLine->text().toInt();
    if(CPUnum<=0)
    {
        QMessageBox* a=new QMessageBox;
        a->setText("CPU数应该大于0");
        a->show();
        on_pauseButton_clicked();
        return;
    }
    if(num<=0)
    {
        QMessageBox* x=new QMessageBox;
        x->setText("道数应该大于0");
        x->show();
        on_pauseButton_clicked();
        return;
    }
    if(CPUnum>num)
    {
        QMessageBox* b=new QMessageBox;
        b->setText("CPU数应小于或等于道数，请重新输入");
        b->show();
        on_pauseButton_clicked();
        return;
    }
    w=new PriorityScheduling(num,CPUnum);
    w->show();
}

void MainWindow::on_pauseButton_clicked()
{
    ui->CPULine->clear();
    ui->numberLine->clear();
}
