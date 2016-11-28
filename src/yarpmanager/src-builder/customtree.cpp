#include "customtree.h"
#include <QMimeData>
#include <QDrag>
#include <qdebug.h>
#include "commons.h"

CustomTree::CustomTree(QWidget *parent) :
    QTreeWidget(parent)
{
    QTreeWidgetItem *it = new QTreeWidgetItem(this,QStringList() << "Modulo");
    it->setData(0,Qt::UserRole,(int)1);
    addTopLevelItem(it);

    QTreeWidgetItem *it1 = new QTreeWidgetItem(this,QStringList() << "Source Port");
    it1->setData(0,Qt::UserRole,(int)2);
    addTopLevelItem(it1);

    QTreeWidgetItem *it2 = new QTreeWidgetItem(this,QStringList() << "Destination Port");
    it2->setData(0,Qt::UserRole,(int)3);
    addTopLevelItem(it2);
    //addTopLevelItem(new QTreeWidgetItem(this,QStringList() << "Model4"));

    setDragEnabled(true);
    setDragDropMode(DragOnly);
}


void CustomTree::mousePressEvent(QMouseEvent *event)
{
    QTreeWidgetItem *selectedItem = currentItem();

    // If the selected Item exists
    if (selectedItem){

        QMimeData *mimeData = new QMimeData;
        //qDebug() << "DRAG " << selectedItem->data(0,Qt::UserRole).toInt();
        mimeData->setText(QString("%1").arg(selectedItem->data(0,Qt::UserRole).toInt()));
        // Create drag
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);


        //qDebug() << "mousePressEvent before exec";
        drag->exec(Qt::CopyAction);
        //qDebug() << "mousePressEvent after exec";
    }

    QTreeWidget::mousePressEvent(event);
}
