#ifndef PRIORITYSCHEDULING_H
#define PRIORITYSCHEDULING_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QVector>
#include "resourse.h"
#define MAX_RUNTIME_RANGE 15
#define MAX_PRIORITY_RANGE 40
#define MAX_MEMORY_RANGE 200

namespace Ui {
class PriorityScheduling;
}

class PriorityScheduling : public QWidget
{
    Q_OBJECT

public:
    explicit PriorityScheduling(int nums,int CPUnums,QWidget *parent = nullptr);
    ~PriorityScheduling();

private slots:
    void on_addButton_clicked();

    void on_suspendButton_clicked();

    void on_notsuspendButton_clicked();

    void on_auto_createButton_clicked();

    void on_startButton_clicked();

    void on_pauseButton_clicked();

    void on_stopAuto_createButton_clicked();

private:
    Ui::PriorityScheduling *ui;
    int CPUnumber;  //CPU数
    int number;     //处理机道数
    bool isAuto_Create; //是否自动创建进程
    QTimer* timer;     //控制主程序运行
    QTimer* timer1;    //控制自动创建进程
    QVector<QString> CPUprocess;   //正在CPU中运行的进程
    QVector<PCB> readyQueue;       //就绪队列
    QVector<PCB> reserveQueue;     //后备队列
    QVector<PCB> suspendQueue;     //挂起队列
    QVector<PCB> finshedQueue;     //完成队列
    memoryNode tableHead;
    QVector<memoryNode*> memoryTable;

private:
    void ShowReadyQueue();       //刷新内存进程表,即就绪队列中的进程
    void ShowCPUprocess();        //展示CPU状态
    void ShowFinshedQueue();     //展示完成队列
    void UpdateProcess();        //每隔1s执行处理机调度程序
    void Auto_CreateProcess();   //自动创建进程
    void ShowReserveQueue();     //刷新外存进程表中后备队列的进程
    void ShowSuspendQueue();     //刷新外存进程表中挂起队列的进程
    bool isReady(PCB);
    bool isReady(int);
    void GoReadyQueue();         //从后备队列中选取进程进入就绪队列

    void ShowPeaceTable(memoryNode* p);       //刷新展示主存分区表
    void InitPeaceTable(memoryNode* p);
    bool InsertProcess(memoryNode* p,int length,QString workName);
    void Mix_1(memoryNode* p);
    void Mix_2(memoryNode* p);
    void getMemoryTable(memoryNode* p,QVector<memoryNode*>& memorytable);
    void DeleteProcess(memoryNode* p,QString workName);
};

#endif // PRIORITYSCHEDULING_H
