#ifndef TRAFFICTHREAD_H
#define TRAFFICTHREAD_H
#include "snmplib.h"
#include <QThread>

class TrafficThread:public QThread
{
    Q_OBJECT
public:
    TrafficThread(SNMPLib *s,QString id);
int inBytes;
int outBytes;
SNMPLib *snmp;
QString ifIndex;
int sleepTime;
bool isPause;
bool isReset;

protected:
    virtual void run();
private:
//    QMutex mutex;
    QThread *thread;

signals:
    void mySignal(QString);
};


#endif // TRAFFICTHREAD_H
