#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <QLabel>

class Node: public QLabel
{
    Q_OBJECT

public:
    void updatePos(int x,int y);
    Node();
    Node(QWidget *parent,QPixmap pixmap,QString address, int x, int y);
    QString address;
};

#endif // NETWORKNODE_H
