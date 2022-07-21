#ifndef RESOURSE_H
#define RESOURSE_H
#include<QString>

//定义PCB结构类
class PCB
{
public:
    QString pid;   //pid
    int runTime;   //要求运行时间
    int doneTime;  //已运行时间
    int priority;  //优先级
    int status;    //进程状态，0：就绪；1：运行；2：等待(阻塞)；3：挂起
    QString prevPCB;  //前驱进程，如果为空则自身为独立进程
    int peace;        //所需主存大小
};

//定义主存分区结点类
class memoryNode
{
public:
    QString workName;  //作业名
    int address;       //首地址
    int length;        //内存长度
    bool state;        //状态，1表示已经被分配，0表示空闲
    memoryNode* left;
    memoryNode* right;

    memoryNode()
    {
        workName="";
        address=0;
        length=0;
        state=0;
        left=nullptr;
        right=nullptr;
    }
};

#endif // RESOURSE_H
