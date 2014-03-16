#include "trafficthread.h"
#include "snmplib.h"
#include <QTime>
#include <QDebug>

TrafficThread::TrafficThread(SNMPLib *s,QString id)
{
    snmp = s;
    ifIndex=id;
    this->thread=new QThread(0);
    sleepTime=1000;
    isPause = false;
    isReset = false;
}

void TrafficThread::run() {
//    mutex.lock();
while (!isPause) {

    QString v;
    QString oid = QObject::tr("1.3.6.1.2.1.2.2.1.10.").append(ifIndex);
    if(snmp->sendOID(oid,&v)){
    qDebug() << "DOWN: " << v;
    }

    oid = QObject::tr("1.3.6.1.2.1.2.2.1.16.").append(ifIndex);
    if(snmp->sendOID(oid,&v)){
    qDebug() << "UP: " << v;
    }
    msleep(sleepTime);
}
//    emit mySignal(this->now);
//    mutex.unlock();
}
