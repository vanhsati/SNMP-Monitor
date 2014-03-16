#include "node.h"
#include <QDebug>
#include <QMouseEvent>

Node::Node(QWidget *parent,QPixmap pixMap,QString address,int x, int y)
    : QLabel(parent)
{
    address=address;
    setPixmap(pixMap);
    move(x,y);
    show();
    setToolTip(address);
    setAttribute(Qt::WA_DeleteOnClose);
}

Node::Node()
{

}
