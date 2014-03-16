#ifndef SNMP_MONITOR_H
#define SNMP_MONITOR_H

#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QLineEdit>
#include "networkwidget.h"
#include "snmplib.h"
#include "trafficthread.h"

namespace Ui {
class SNMPMonitor;
}

class SNMPMonitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit SNMPMonitor(QWidget *parent = 0);
    ~SNMPMonitor();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::SNMPMonitor *ui;
    NetworkWidget *networkWidget;
    SNMPLib *snmpMonitor;
    QString ifIndexMonitor;
    TrafficThread *trafficThread;
    QDialog *configDialog;
    void getDeviceInfo(SNMPLib *snmp,QString title,QString oid);
    void getListInterface(SNMPLib *snmp,int ifNumber);
    void getInterfaceInfo(SNMPLib *snmp,QTreeWidgetItem *item,QString title,QString oid);
    QTreeWidgetItem *ifTreeItem;
    bool isConnectError;
};

#endif // SNMP_MONITOR_H
