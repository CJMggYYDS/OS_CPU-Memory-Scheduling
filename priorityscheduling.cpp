#include <QMessageBox>
#include <QDebug>
#include <algorithm>
#include "priorityscheduling.h"
#include "ui_priorityscheduling.h"
using namespace std;

PriorityScheduling::PriorityScheduling(int nums,int CPUnums,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PriorityScheduling)
{
    ui->setupUi(this);
    number=nums;
    CPUnumber=CPUnums;
    ui->cpu_label->setText("CPU数："+QString::number(CPUnums));
    ui->num_label->setText("道数："+QString::number(nums));
    this->setWindowTitle("CJM's CPU-Priority-Scheduling Program");
    isAuto_Create=false;
    for(int i=0;i<CPUnumber;i++)
    {
        CPUprocess.append("空");
    }

    InitPeaceTable(&tableHead);
    ShowReadyQueue();
    ShowSuspendQueue();
    ShowReserveQueue();
    GoReadyQueue();
    ShowPeaceTable(&tableHead);
    ShowCPUprocess();
    ShowFinshedQueue();
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&PriorityScheduling::UpdateProcess);
    timer->start(1000);
    timer1=new QTimer(this);
    connect(timer1,&QTimer::timeout,this,&PriorityScheduling::Auto_CreateProcess);
    timer1->start(2000);
    setFixedSize(this->width(),this->height());
}

PriorityScheduling::~PriorityScheduling()
{
    delete ui;
}

void PriorityScheduling::InitPeaceTable(memoryNode* p)
{
    p->address = 100;
    p->length = 1000;
    p->state = 0;
    p->workName = "";
    p->left=nullptr;
    p->right=nullptr;
    qDebug()<<"初始化完成！";
}

bool PriorityScheduling::InsertProcess(memoryNode *p, int length, QString workName)
{
    if(p!=nullptr)
    {
        if((p->state==0)&&(length<=p->length)&&(p->left==nullptr)&&(p->right==nullptr))
        {
            memoryNode* m1=new memoryNode();
            memoryNode* m2=new memoryNode();
            m1->workName=workName;
            m2->workName="";
            m1->address=p->address;
            m1->state=1;
            m1->length=length;
            m2->address=m1->address+length;
            m2->length=p->length-m1->length;
            m2->state=0;

            p->left=m1;
            p->right=m2;

            m1->left=nullptr;
            m1->right=nullptr;
            m2->left=nullptr;
            m2->right=nullptr;
            p->state=1;
            return true;
        }
        else {
            return InsertProcess(p->left,length,workName) || InsertProcess(p->right,length,workName);
        }
    }
    return false;
}

void PriorityScheduling::getMemoryTable(memoryNode *p, QVector<memoryNode *> &memorytable)
{
    if(p!=nullptr)
    {
        if(p->left==nullptr && p->right==nullptr)
        {
            memorytable.append(p);
        }
        if(p->left) getMemoryTable(p->left,memorytable);
        if(p->right) getMemoryTable(p->right,memorytable);
    }
}

void PriorityScheduling::Mix_1(memoryNode *p)
{
    if(p!=nullptr && p->left!=nullptr)
    {
        if(p->left->left==nullptr && p->right->right==nullptr && p->left->state==0 && p->right->state==0)
        {
            p->state=0;
            auto x=p->left;
            auto y=p->right;
            p->left=nullptr;
            p->right=nullptr;
            delete x;
            delete y;
        }
        Mix_1(p->left);
        Mix_1(p->right);
    }
}

void PriorityScheduling::Mix_2(memoryNode *p)
{
    if(p->left!=nullptr && p->right->left!=nullptr)
    {
        if(p->left->state==0&&p->right->left->state==0)
        {
            p->left->length=p->right->right->address;
            p->right=p->right->right;
        }
        Mix_2(p->left);
        Mix_2(p->right);
    }
}

void PriorityScheduling::DeleteProcess(memoryNode *p, QString workName)
{
    if(p!=nullptr)
    {
        if(p->workName==workName)
        {
            p->workName="";
            p->state=0;
        }
        DeleteProcess(p->left,workName);
        DeleteProcess(p->right,workName);
    }
}

void PriorityScheduling::ShowPeaceTable(memoryNode *p)
{
    QVector<memoryNode*>memory;
    getMemoryTable(p,memory);
    ui->peaceTable->clear();
    ui->peaceTable->setWindowTitle("主存分区表");
    int size=memory.size();
    ui->peaceTable->setColumnCount(6);
    ui->peaceTable->setRowCount(size+10);
    ui->peaceTable->setHorizontalHeaderLabels(QStringList()<<"序号"<<"进程名"<<"开始地址"<<"结束地址"<<"长度"<<"状态");
    ui->peaceTable->verticalHeader()->setVisible(false);
    ui->peaceTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->peaceTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QTableWidgetItem* i0=new QTableWidgetItem("#");
    QTableWidgetItem* i1=new QTableWidgetItem("操作系统进程");
    QTableWidgetItem* i2=new QTableWidgetItem("0");
    QTableWidgetItem* i3=new QTableWidgetItem("100");
    QTableWidgetItem* i4=new QTableWidgetItem("100");
    QTableWidgetItem* i5=new QTableWidgetItem("正在使用");
    ui->peaceTable->setItem(0,0,i0);
    ui->peaceTable->setItem(0,1,i1);
    ui->peaceTable->setItem(0,2,i2);
    ui->peaceTable->setItem(0,3,i3);
    ui->peaceTable->setItem(0,4,i4);
    ui->peaceTable->setItem(0,5,i5);
    for(int i=0;i<memory.size();i++)
    {
        auto m=memory[i];
        QTableWidgetItem* item0=new QTableWidgetItem(QString::number(i,10));
        QTableWidgetItem* item1=new QTableWidgetItem(m->workName);
        QTableWidgetItem* item2=new QTableWidgetItem(QString::number(m->address,10));
        QTableWidgetItem* item3=new QTableWidgetItem(QString::number(m->address+m->length),10);
        QTableWidgetItem* item4=new QTableWidgetItem(QString::number(m->length,10));
        QString state;
        if(m->state==0)
            state="空闲";
        else
            state="正在使用";
        QTableWidgetItem* item5=new QTableWidgetItem(state);
        ui->peaceTable->setItem(i+1,0,item0);
        ui->peaceTable->setItem(i+1,1,item1);
        ui->peaceTable->setItem(i+1,2,item2);
        ui->peaceTable->setItem(i+1,3,item3);
        ui->peaceTable->setItem(i+1,4,item4);
        ui->peaceTable->setItem(i+1,5,item5);
    }
    ui->peaceTable->show();
    memoryTable=memory;
}

void PriorityScheduling::ShowReserveQueue()
{
    ui->outpeaceTable->clear();
    ui->outpeaceTable->setWindowTitle("后备队列");
    int size=reserveQueue.size();
    ui->outpeaceTable->setColumnCount(5);
    ui->outpeaceTable->setRowCount(size+5);
    ui->outpeaceTable->setHorizontalHeaderLabels(QStringList() << "pid" << "剩余运行时间" << "已运行时间" << "优先级" << "状态");
    ui->outpeaceTable->verticalHeader()->setVisible(false);
    ui->outpeaceTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->outpeaceTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for(int i=0;i<size;i++)
    {
        PCB pcb=reserveQueue[i];
        QTableWidgetItem* item1=new QTableWidgetItem(pcb.pid);
        QTableWidgetItem* item2=new QTableWidgetItem(QString::number(pcb.runTime-pcb.doneTime,10));
        QTableWidgetItem* item3=new QTableWidgetItem(QString::number(pcb.doneTime,10));
        QTableWidgetItem* item4=new QTableWidgetItem(QString::number(pcb.priority,10));
        QTableWidgetItem* item5=new QTableWidgetItem("new/ready");

        ui->outpeaceTable->setItem(i,0,item1);
        ui->outpeaceTable->setItem(i,1,item2);
        ui->outpeaceTable->setItem(i,2,item3);
        ui->outpeaceTable->setItem(i,3,item4);
        ui->outpeaceTable->setItem(i,4,item5);
    }
}

void PriorityScheduling::ShowSuspendQueue()
{
    ui->outpeaceTable2->clear();
    ui->outpeaceTable2->setWindowTitle("挂起队列");
    int size=suspendQueue.size();
    ui->outpeaceTable2->setColumnCount(5);
    ui->outpeaceTable2->setRowCount(size+5);
    ui->outpeaceTable2->setHorizontalHeaderLabels(QStringList() << "pid" << "剩余运行时间" << "已运行时间" << "优先级" << "状态");
    ui->outpeaceTable2->verticalHeader()->setVisible(false);
    ui->outpeaceTable2->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->outpeaceTable2->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for(int i=0;i<size;i++)
    {
        PCB pcb=suspendQueue[i];
        QTableWidgetItem* item1=new QTableWidgetItem(pcb.pid);
        QTableWidgetItem* item2=new QTableWidgetItem(QString::number(pcb.runTime-pcb.doneTime,10));
        QTableWidgetItem* item3=new QTableWidgetItem(QString::number(pcb.doneTime,10));
        QTableWidgetItem* item4=new QTableWidgetItem(QString::number(pcb.priority,10));
        QTableWidgetItem* item5=new QTableWidgetItem("suspended");

        ui->outpeaceTable2->setItem(i,0,item1);
        ui->outpeaceTable2->setItem(i,1,item2);
        ui->outpeaceTable2->setItem(i,2,item3);
        ui->outpeaceTable2->setItem(i,3,item4);
        ui->outpeaceTable2->setItem(i,4,item5);
    }
}

void PriorityScheduling::ShowReadyQueue()
{
    ui->inpeaceTable->clear();
    ui->inpeaceTable->setWindowTitle("就绪队列");
    int size=readyQueue.size();
    ui->inpeaceTable->setColumnCount(5);
    ui->inpeaceTable->setRowCount(size+5);
    ui->inpeaceTable->setHorizontalHeaderLabels(QStringList() << "pid" << "剩余运行时间" << "已运行时间" << "优先级" << "状态");
    ui->inpeaceTable->verticalHeader()->setVisible(false);
    ui->inpeaceTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->inpeaceTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for(int i=0;i<size;i++)
    {
        PCB pcb=readyQueue[i];
        QTableWidgetItem* item1=new QTableWidgetItem(pcb.pid);
        QTableWidgetItem* item2=new QTableWidgetItem(QString::number(pcb.runTime-pcb.doneTime,10));
        QTableWidgetItem* item3=new QTableWidgetItem(QString::number(pcb.doneTime,10));
        QTableWidgetItem* item4=new QTableWidgetItem(QString::number(pcb.priority,10));
        QString status;
        if(pcb.status==0)
        {
            status="ready";
        }
        else if(pcb.status==1)
        {
            status="running";
        }
        else
        {
            status="waiting";
        }
        QTableWidgetItem* item5=new QTableWidgetItem(status);

        ui->inpeaceTable->setItem(i,0,item1);
        ui->inpeaceTable->setItem(i,1,item2);
        ui->inpeaceTable->setItem(i,2,item3);
        ui->inpeaceTable->setItem(i,3,item4);
        ui->inpeaceTable->setItem(i,4,item5);

        readyQueue[i].status=0;
    }

}

void PriorityScheduling::ShowFinshedQueue()
{
    int size=finshedQueue.size();
    ui->finshTable->clear();
    ui->finshTable->setWindowTitle("完成队列");
    ui->finshTable->setColumnCount(2);
    ui->finshTable->setRowCount(size);
    ui->finshTable->setHorizontalHeaderLabels(QStringList() << "已完成进程" << "进程运行时间");
    ui->finshTable->verticalHeader()->setVisible(false);
    ui->finshTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->finshTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for(int i=0;i<size;i++)
    {
        PCB p=finshedQueue[i];
        QTableWidgetItem* item0=new QTableWidgetItem(p.pid);
        QTableWidgetItem* item1=new QTableWidgetItem(QString::number(p.runTime,10));

        ui->finshTable->setItem(i,0,item0);
        ui->finshTable->setItem(i,1,item1);
    }
}

void PriorityScheduling::ShowCPUprocess()
{
    ui->cpuTable->clear();
    ui->cpuTable->setWindowTitle("CPU运行进程");
    ui->cpuTable->setColumnCount(2);
    ui->cpuTable->setRowCount(CPUnumber);
    ui->cpuTable->setHorizontalHeaderLabels(QStringList() << "CPU" << "正在运行进程");
    ui->cpuTable->verticalHeader()->setVisible(false);
    ui->cpuTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->cpuTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for(int i=0;i<CPUnumber;i++)
    {
        QTableWidgetItem* item0=new QTableWidgetItem("CPU"+QString::number(i+1),10);
        QTableWidgetItem* item1=new QTableWidgetItem(CPUprocess[i]);

        ui->cpuTable->setItem(i,0,item0);
        ui->cpuTable->setItem(i,1,item1);
    }

    for(int i=0;i<CPUnumber;i++)
    {
        CPUprocess[i]="空";
    }
}

void PriorityScheduling::Auto_CreateProcess()
{
    if(isAuto_Create)
    {
        PCB pcb;
        pcb.pid="RP-"+QString::number(qrand()%1000+1,10);
        pcb.priority=qrand()%MAX_PRIORITY_RANGE+1;
        pcb.runTime=qrand()%MAX_RUNTIME_RANGE+1;
        pcb.doneTime=0;
        pcb.status=0; //ready
        pcb.peace=qrand()%MAX_MEMORY_RANGE+1;

        qDebug()<<"自动添加进程："<<pcb.pid;
        reserveQueue.append(pcb);
        ShowReserveQueue();
        ShowPeaceTable(&tableHead);
    }
}

void PriorityScheduling::GoReadyQueue()
{
    Mix_1(&tableHead);
    Mix_2(&tableHead);
    for(int i=0;i<reserveQueue.size()&&readyQueue.size()<number;i++)
    {
        if(InsertProcess(&tableHead,reserveQueue[i].peace,reserveQueue[i].pid))
        {
            readyQueue.append(reserveQueue[i]);
            reserveQueue.remove(i,1);
            i--;
        }
    }
}

bool PriorityScheduling::isReady(PCB p)
{
    if(p.prevPCB=="")
    {
        return true;
    }
    for(PCB item:finshedQueue)
    {
        if(item.pid==p.prevPCB)
        {
            return true;
        }
    }
    return false;
}

bool PriorityScheduling::isReady(int i)
{
    for(PCB item:finshedQueue)
    {
        if(item.pid==suspendQueue[i].prevPCB)
        {
            return true;
        }
    }
    return false;
}

void PriorityScheduling::UpdateProcess()
{
    sort(reserveQueue.begin(),reserveQueue.end(),[](const PCB& a,const PCB&b){
        return a.priority>b.priority||(a.priority==b.priority&&a.runTime-a.doneTime>b.runTime-b.doneTime);
    });
    GoReadyQueue();
    ShowPeaceTable(&tableHead);
    ShowReadyQueue();

    if(!readyQueue.empty())
    {
        int CPU_flag[CPUnumber];
        for(int j=0;j<CPUnumber;j++)
        {
            CPU_flag[j]=0;
        }
        for(int i=0;i<readyQueue.size();i++)
        {
            readyQueue[i].priority++;
        }
        for(int k=0;k<CPUnumber;k++)
        {
            int n=0;
            while(CPU_flag[k]==0 && n<readyQueue.size())
            {
                while(n<readyQueue.size() && readyQueue[n].status!=0)
                {
                    n++;
                }
                if(n>=readyQueue.size())
                    break;

                if(readyQueue[n].status==0 && isReady(readyQueue[n]) && n<readyQueue.size())
                {
                    PCB pcb=readyQueue[n];
                    CPUprocess[k]=pcb.pid;
                    readyQueue.remove(n,1);
                    pcb.status=1;
                    pcb.doneTime++;

                    if(pcb.doneTime==pcb.runTime)
                    {
                        finshedQueue.append(pcb);
                        qDebug()<<"进程"<<pcb.pid<<"执行完成";
                        DeleteProcess(&tableHead,pcb.pid);
                        Mix_1(&tableHead);
                        Mix_2(&tableHead);

                        for(int x=0;x<suspendQueue.size();x++)
                        {
                            if(isReady(x))
                            {
                                suspendQueue[x].status=0;
                                reserveQueue.append(suspendQueue[x]);
                                suspendQueue.remove(x,1);
                                break;
                            }
                        }
                    }
                    else
                    {
                        readyQueue.append(pcb);
                    }
                    CPU_flag[k]=1;
                }
                else
                {
                    readyQueue[n].status=2;
                }
            }
            if(CPU_flag[k]==0 && !readyQueue.empty() && !reserveQueue.empty())
            {
                readyQueue[0].status=3; //
                suspendQueue.append(readyQueue[0]);
                DeleteProcess(&tableHead,readyQueue[0].pid);
                Mix_1(&tableHead);
                Mix_2(&tableHead);
                readyQueue.pop_front();
                GoReadyQueue();
            }

        }
    }
    sort(readyQueue.begin(),readyQueue.end(),[](const PCB& a,const PCB&b){
        return a.priority>b.priority||(a.priority==b.priority&&a.runTime-a.doneTime>b.runTime-b.doneTime);
    });
    Mix_1(&tableHead);
    Mix_2(&tableHead);
    ShowPeaceTable(&tableHead);
    ShowReadyQueue();
    ShowReserveQueue();
    ShowSuspendQueue();
    ShowFinshedQueue();
    ShowCPUprocess();
}

void PriorityScheduling::on_addButton_clicked()
{
    PCB p;
    p.pid="P-"+QString::number(qrand()%1000+1,10);
    p.priority=qrand()%MAX_PRIORITY_RANGE+1;
    p.runTime=qrand()%MAX_RUNTIME_RANGE+1;
    p.doneTime=0;
    p.status=0;     //ready
    p.peace=qrand()%MAX_MEMORY_RANGE+1;
    bool isExist=false;
    for(int i=0;i<readyQueue.size();i++)
    {
        if(p.pid==readyQueue[i].pid)
            isExist=true;
    }
    for(int j=0;j<reserveQueue.size();j++)
    {
        if(p.pid==reserveQueue[j].pid)
            isExist=true;
    }
    if(isExist)
    {
        qDebug()<<"进程"<<p.pid<<"添加失败";
        QMessageBox* b=new QMessageBox();
        b->setWindowTitle("错误");
        b->setText("该进程的PCB已存在,添加进程失败");
        b->exec();
    }
    else
    {
        qDebug()<<"进程"<<p.pid<<"添加成功";
        reserveQueue.append(p);
        ShowReserveQueue();
        ShowPeaceTable(&tableHead);
    }
}

void PriorityScheduling::on_suspendButton_clicked()
{
    QString PCBid=ui->suspendLine->text();
    int flag=0;
    for(int i=0;i<readyQueue.size();i++)
    {
        if(PCBid==readyQueue[i].pid)
        {
            flag=1;
            readyQueue[i].status=3;
            suspendQueue.append(readyQueue[i]);
            DeleteProcess(&tableHead,readyQueue[i].pid);
            Mix_1(&tableHead);
            Mix_2(&tableHead);
            readyQueue.remove(i,1);
            break;
        }
    }
    if(flag==0)
    {
        qDebug()<<"就绪队列无要挂起的进程，挂起失败";
    }
    else
    {
        qDebug()<<"挂起成功";
    }
    ui->suspendLine->clear();
}

void PriorityScheduling::on_notsuspendButton_clicked()
{
    QString PCBid=ui->notsuspendLine->text();
    int flag=0;
    for(int i=0;i<suspendQueue.size();i++)
    {
        if(PCBid==suspendQueue[i].pid)
        {
            flag=1;
            suspendQueue[i].status=1;
            reserveQueue.append(suspendQueue[i]);
            suspendQueue.remove(i,1);
        }
    }
    if(flag==0)
    {
        qDebug()<<"挂起队列无该进程，解挂失败";
    }
    else
    {
        qDebug()<<"解挂成功";
    }
    ui->notsuspendLine->clear();
}

void PriorityScheduling::on_auto_createButton_clicked()
{
    if(isAuto_Create==false)
    {
        isAuto_Create=true;
    }
}

void PriorityScheduling::on_stopAuto_createButton_clicked()
{
    if(isAuto_Create==true)
    {
        isAuto_Create=false;
    }
}

void PriorityScheduling::on_startButton_clicked()
{
    timer->start();
}

void PriorityScheduling::on_pauseButton_clicked()
{
    timer->stop();
}


