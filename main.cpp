#include "snmpmonitor.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SNMPMonitor w;
    w.show();
    return a.exec();
}
