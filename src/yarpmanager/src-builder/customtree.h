#ifndef CUSTOMTREE_H
#define CUSTOMTREE_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <yarp/conf/compiler.h>

class CustomTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTree(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:

};

#endif // CUSTOMTREE_H
