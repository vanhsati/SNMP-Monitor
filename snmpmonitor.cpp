#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include "snmpmonitor.h"
#include "ui_snmpmonitor.h"
#include "networkwidget.h"
#include "snmplib.h"
#include <QDebug>
#include <QThread>

SNMPMonitor::SNMPMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SNMPMonitor)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("SNMP Monitor"));
    ui->treeWidget->setColumnCount(2);
    ui->lineEdit->setText("localhost");
    ui->lineEdit_2->setText("public");
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ifIndexMonitor = "";

    // create networkWidget
    networkWidget = new NetworkWidget(this,ui->lineEdit);
    networkWidget->setMinimumHeight(250);
    networkWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(ui->networkTopologyWidget);
    vlayout->addWidget(networkWidget);
    vlayout->addWidget(ui->widget1);
    ui->frame2->setLayout(vlayout);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(ui->frame1);
    hlayout->addWidget(ui->frame2);

    ui->centralWidget->setLayout(hlayout);

    configDialog = new QDialog(this);
    NetworkWidget *configWidget = new NetworkWidget(this,ui->lineEdit);
    configWidget->initConfigWidget();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(configWidget);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    configDialog->setWindowTitle("Configuration");
    configDialog->setLayout(layout);
    configDialog->move(x()+width(),y());
    configDialog->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint);
}

SNMPMonitor::~SNMPMonitor()
{
    delete ui;
}

void SNMPMonitor::on_pushButton_clicked()
{
    isConnectError = false;
    SNMPLib *snmp = new SNMPLib();
    for (int i = ui->treeWidget->children().size()-1;i>=0;i--){
        ui->treeWidget->takeTopLevelItem(i);
    }
    if(snmp->init(ui->lineEdit->text(),ui->lineEdit_2->text()))
    {
        getDeviceInfo(snmp,"sysDescr","1.3.6.1.2.1.1.1");
        getDeviceInfo(snmp,"sysUptime","1.3.6.1.2.1.1.3");
        getDeviceInfo(snmp,"sysName","1.3.6.1.2.1.1.4");
        getDeviceInfo(snmp,"sysContact","1.3.6.1.2.1.1.5");
        getDeviceInfo(snmp,"sysLocation","1.3.6.1.2.1.1.6");
        getDeviceInfo(snmp,"ifNumber","1.3.6.1.2.1.2.1");
        getListInterface(snmp,QString(ifTreeItem->text(1)).toInt());
        snmp->closeSession();
    }
}

void SNMPMonitor::getDeviceInfo(SNMPLib *snmp,QString title,QString oid)
{
    QString v;
    QString oid2=oid.append(".0");
    if((snmp->sendOID(oid,&v)||snmp->sendOID(oid2,&v))&&!isConnectError){
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget);
        itm->setText(0,title);
        itm->setText(1,v);
        ui->treeWidget->addTopLevelItem(itm);
        if (title=="ifNumber") ifTreeItem = itm;
    }
    else isConnectError = true;
}

void SNMPMonitor::getListInterface(SNMPLib *snmp,int ifNumber)
{
    QString v;
    for (int i=1;i<=ifNumber;i++)
        if (!isConnectError)
        {
            if (snmp->sendOID("1.3.6.1.2.1.2.2.1.2."+QString::number(i),&v))
            {
                QTreeWidgetItem *itm = new QTreeWidgetItem(ifTreeItem);
                itm->setText(0,v);
                itm->setText(1,QString::number(i));
                ifTreeItem->addChild(itm);
            }
            else isConnectError = true;
        }
}

void SNMPMonitor::on_pushButton_4_clicked()
{
    if (ui->pushButton_4->text()=="Config"){
        ui->pushButton_4->setText("Finish");
        networkWidget->isConfigMode = true;
        configDialog->move(x()+width(),y());
        configDialog->show();
    } else {
        configDialog->hide();
        ui->pushButton_4->setText("Config");
        networkWidget->isConfigMode = false;
    }
}

//start
void SNMPMonitor::on_pushButton_2_clicked()
{
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(true);
//    trafficThread->start();

}

//stop
void SNMPMonitor::on_pushButton_3_clicked()
{
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
//    trafficThread->quit();
}

void SNMPMonitor::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item->parent()==ifTreeItem){
        if(!ui->pushButton_2->isEnabled()&&!ui->pushButton_3->isEnabled())
            ui->pushButton_2->setEnabled(true);
        SNMPLib *snmp = new SNMPLib();
        for(int i=item->childCount();i>=0;i--) item->removeChild(item->child(0));
        if(snmp->init(ui->lineEdit->text(),ui->lineEdit_2->text()))
        {
            getInterfaceInfo(snmp,item,"Type","1.3.6.1.2.1.2.2.1.3.");
            getInterfaceInfo(snmp,item,"Speed","1.3.6.1.2.1.2.2.1.5.");
            getInterfaceInfo(snmp,item,"PhysicalAddress","1.3.6.1.2.1.2.2.1.6.");
            getInterfaceInfo(snmp,item,"AdminStatus","1.3.6.1.2.1.2.2.1.7.");
            getInterfaceInfo(snmp,item,"OperationStatus","1.3.6.1.2.1.2.2.1.8.");
            getInterfaceInfo(snmp,item,"InBytes","1.3.6.1.2.1.2.2.1.10.");
            getInterfaceInfo(snmp,item,"OutByte","1.3.6.1.2.1.2.2.1.16.");

            snmp->closeSession();
        }

//        if(ifIndexMonitor == "")
//        {
//            ifIndexMonitor = item->text(0);
//            snmpMonitor->init(ui->lineEdit->text(),ui->lineEdit_2->text());
//            trafficThread = new TrafficThread(snmpMonitor,ifIndexMonitor);
//        }
//        else if(ifIndexMonitor != item->text(0))
//        {
//            if (trafficThread->isRunning()){
//                trafficThread->quit();
//                snmpMonitor->closeSession();
//                ui->pushButton_2->setEnabled(false);
//                ui->pushButton_3->setEnabled(false);
//            }
//            else {
//                ifIndexMonitor = item->text(0);
//                snmpMonitor->init(ui->lineEdit->text(),ui->lineEdit_2->text());
//                trafficThread = new TrafficThread(snmpMonitor,ifIndexMonitor);
//            }
//        }
    }
}

void SNMPMonitor::getInterfaceInfo(SNMPLib *snmp,QTreeWidgetItem *item,QString title,QString oid)
{
    QString v;
    QTreeWidgetItem *itm = new QTreeWidgetItem(item);
    QString oid2 = oid.append(item->text(1));
    if(snmp->sendOID(oid2,&v)){
        itm->setText(0,title);
        itm->setText(1,v);
        item->addChild(itm);
    }
}
