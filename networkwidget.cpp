/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QDialog>
#include <QList>
#include <QInputDialog>
#include "networkwidget.h"
#include "snmpmonitor.h"


//! [0]
NetworkWidget::NetworkWidget(QWidget *parent, QLineEdit *qLine)
    : QWidget(parent)
{
    isNetworkWidget = true;
    isConfigMode = false;
    lineEdit = qLine;
    setAcceptDrops(true);
}

QStringList NetworkWidget::templateNodeList = QStringList()<<"imac"<<"pc"<<"switch"<<"router"<<"laptop";

void NetworkWidget::initConfigWidget()
{
    setFixedSize(84,74*templateNodeList.size()+20);
    isNetworkWidget = false;
    setToolTip("Drag & drop");
    QString nodeIconPath;
    QString nodeDescription;
    for (int i=0;i<templateNodeList.size();i++){
        nodeDescription=templateNodeList.at(i).toLocal8Bit().constData();
        nodeIconPath=QString(":images/%1.png").arg(nodeDescription);
        addNode(nodeIconPath,nodeDescription,10,10+i*74);
    }
}

void NetworkWidget::paintEvent(QPaintEvent *p)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::blue, 2));
    QPoint p1, p2;
    for (int i=0;i<linkList.size();i++){
//        p1 = lineList.at(i).l1->pos()+lineList.at(i).p1;
//        p2 = lineList.at(i).l2->pos()+lineList.at(i).p2;
        p1 = linkList.at(i).n1->pos()+QPoint(32,32);
        p2 = linkList.at(i).n2->pos()+QPoint(32,32);
        painter.drawLine(p1,p2);
    }
}

void NetworkWidget::addNode(QString nodeIconPath, QString nodeDescription, int x, int y)
{
    new Node(this,QPixmap(nodeIconPath),nodeDescription,x,y);
}

void NetworkWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void NetworkWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void NetworkWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata") && isNetworkWidget) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset;
        QPoint newPos = event->pos()-offset;
        Node *node = new Node(this,pixmap,selectedAddress,newPos.x(),newPos.y());

        if (event->source() == this && selectedNode!=NULL) {
            for (int i=0;i<linkList.size();i++){
                if (linkList.at(i).n1==selectedNode){
                    Link tempLink = linkList.at(i);
                    tempLink.n1=node;
                    linkList.replace(i,tempLink);
                }
                if (linkList.at(i).n2==selectedNode){
                    Link tempLink = linkList.at(i);
                    tempLink.n2=node;
                    linkList.replace(i,tempLink);
                }
            }
            event->setDropAction(Qt::MoveAction);
            event->accept();
            update();
        } else {
                event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }

}

void NetworkWidget::mousePressEvent(QMouseEvent *event)
{

    Node *node = static_cast<Node*>(childAt(event->pos()));
    if (!node) return;
    selectedNode=node;
    selectedAddress=node->address;

    if (event->button()==Qt::LeftButton && (isConfigMode||!isNetworkWidget)){
        QPixmap pixmap = *node->pixmap();
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixmap << QPoint(event->pos() - node->pos());

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-dnditemdata", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);
        drag->setHotSpot(event->pos() - node->pos());

        QPixmap tempPixmap = pixmap;
        QPainter painter;
        painter.begin(&tempPixmap);
        painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
        painter.end();

        node->setPixmap(tempPixmap);

        if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
            node->close();
        } else {
                node->show();
                node->setPixmap(pixmap);
        }
    } else if (event->button()==Qt::LeftButton && !isConfigMode && isNetworkWidget)
    {
        lineEdit->setText(selectedAddress);
    } else if (event->button()==Qt::RightButton && isConfigMode && isNetworkWidget){
        newLink = new Link();
        newLink->n1 = node;
        newLink->p1 = event->pos() - node->pos();
        isLinking = true;
    }
}

void NetworkWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button()==Qt::RightButton && isConfigMode && isLinking)
    {
        isLinking = false;
        Node *node = static_cast<Node*>(childAt(event->pos()));
        if (!node) {
            free(newLink);
            return;
        }
        newLink->n2 = node;
        newLink->p2 = event->pos() - node->pos();
        if (newLink->n2!=newLink->n1) linkList.append(*newLink);
        update();
    }
}

void NetworkWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (isConfigMode && isNetworkWidget)
    {
        Node *node = static_cast<Node*>(childAt(event->pos()));
        if (!node) return;

        bool ok;
        QString text = QInputDialog::getText(this, tr(""),
                                              tr("Enter device address"), QLineEdit::Normal,
                                              node->address, &ok);
        if (ok && !text.isEmpty()) {
            node->address = text;
            node->setToolTip(text);
        }
    }
}
