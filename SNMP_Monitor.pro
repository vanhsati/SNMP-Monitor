#-------------------------------------------------
#
# Project created by QtCreator 2014-03-02T04:11:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SNMP_Monitor
TEMPLATE = app


SOURCES += main.cpp\
    snmpmonitor.cpp \
    snmplib.cpp \
    networkwidget.cpp \
    node.cpp \
    link.cpp \
    trafficthread.cpp

HEADERS  += \
    snmpmonitor.h \
    snmplib.h \
    networkwidget.h \
    node.h \
    link.h \
    trafficthread.h

FORMS    += \
    snmpmonitor.ui

RESOURCES += \
    icons.qrc

LIBS += -L"/usr/include/net-snmp"d

unix|win32: LIBS += -lnetsnmp
