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
    ui->lineEdit->setText("192.168.1.1");
    ui->lineEdit_2->setText("public");
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
//    ifIndexMonitor = "";

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

    snmp = new SNMPLib();
    currentIfIndex = -1;
}

SNMPMonitor::~SNMPMonitor()
{
    delete ui;
}

void SNMPMonitor::on_pushButton_clicked()
{
    isConnectError = false;
//    SNMPLib *snmp = new SNMPLib();
    for (int i = ui->treeWidget->children().size()-1;i>=0;i--){
        ui->treeWidget->takeTopLevelItem(i);
    }
    if(snmp->init(ui->lineEdit->text(),ui->lineEdit_2->text()))
    {
        getDeviceInfo("sysDescr","1.3.6.1.2.1.1.1");
        getDeviceInfo("sysUptime","1.3.6.1.2.1.1.3");
        getDeviceInfo("sysName","1.3.6.1.2.1.1.4");
        getDeviceInfo("sysContact","1.3.6.1.2.1.1.5");
        getDeviceInfo("sysLocation","1.3.6.1.2.1.1.6");
        getDeviceInfo("ifNumber","1.3.6.1.2.1.2.1");
        getListInterface(QString(ifTreeItem->text(1)).toInt());
//        snmp->closeSession();
    }
}

void SNMPMonitor::getDeviceInfo(QString title,QString oid)
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

void SNMPMonitor::getListInterface(int ifNumber)
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
                getInterfaceInfo(itm,"Type","1.3.6.1.2.1.2.2.1.3.");
                getInterfaceInfo(itm,"Speed","1.3.6.1.2.1.2.2.1.5.");
                getInterfaceInfo(itm,"PhysicalAddress","1.3.6.1.2.1.2.2.1.6.");
                getInterfaceInfo(itm,"AdminStatus","1.3.6.1.2.1.2.2.1.7.");
                getInterfaceInfo(itm,"OperationStatus","1.3.6.1.2.1.2.2.1.8.");
                getInterfaceInfo(itm,"InBytes","1.3.6.1.2.1.2.2.1.10.");
                getInterfaceInfo(itm,"OutByte","1.3.6.1.2.1.2.2.1.16.");
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

    if (trafficThread->isReset){
        trafficThread->destroyed();
        trafficThread = new TrafficThread(snmp,currentIfIndex);
    }
    trafficThread->start();
}

//stop
void SNMPMonitor::on_pushButton_3_clicked()
{
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    trafficThread->isPause = true;
}

void SNMPMonitor::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
}

void SNMPMonitor::getInterfaceInfo(QTreeWidgetItem *item,QString title,QString oid)
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

void SNMPMonitor::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if(item->parent()==ifTreeItem){
        if (item->text(1) != currentIfIndex){
            currentIfIndex = item->text(1);
            ui->pushButton_2->setEnabled(true);
            ui->pushButton_3->setEnabled(false);
            trafficThread->isPause = true;
            trafficThread->isReset = true;
        }
    }
}
