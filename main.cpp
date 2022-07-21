#include "priorityscheduling.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PriorityScheduling* w=new PriorityScheduling(5,1);
    w->show();


    return a.exec();
}
