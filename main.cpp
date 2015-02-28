#include "managementapplcation.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ManagementApplcation w;
    w.show();
    w.StartUP();

    return a.exec();
}
