/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>
#include <QMenu>
#include <yarp/conf/compiler.h>


/*! \class CustomTreeWidgetItem
    \brief A custom QTreeWidgetItem

    This class is an override of the original QTreeWidgetItem reimplementing the sort
    function using the numerical order for the ID columns.
*/
class CustomTreeWidgetItem : public QTreeWidgetItem
{
public:
    CustomTreeWidgetItem(QTreeWidget *view, const QStringList &strings, int type = Type) :
        QTreeWidgetItem(view,strings,type){}

    CustomTreeWidgetItem(QTreeWidgetItem *item, const QStringList &strings, int type = Type) :
        QTreeWidgetItem(item,strings,type){}
    bool operator< (const QTreeWidgetItem &other) const override {
        int sortCol = treeWidget()->sortColumn();
        if(sortCol == 1){
            int myNumber = text(sortCol).toInt();
            int otherNumber = other.text(sortCol).toInt();
            return myNumber < otherNumber;
        }else{
            return QTreeWidgetItem::operator <(other);
        }
    }

};

/*! \class CustomTreeWidget
    \brief A custom QTreeWidget

    This class is an override of the original QTreeWidget expanding some functionalities.
*/
class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTreeWidget(QWidget *parent = 0);
    void setContextMenu(QMenu *);

protected:
    void mousePressEvent(QMouseEvent *e) override;

private:
    Qt::SortOrder sort;
    QMenu *contextMenu;
signals:

private slots:
    void onHeaderClicked(int index);
    void onConnContext(QPoint p);

};

#endif // CUSTOMTREEWIDGET_H
