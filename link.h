#ifndef LINK_H
#define LINK_H
#include <QPoint>
#include <node.h>

class Link
{
public:
    Link();
    Node *n1,*n2;
    QPoint p1, p2;
};

#endif // LINK_H
