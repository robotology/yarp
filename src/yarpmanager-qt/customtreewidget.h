/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>
#include <QMenu>


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

    bool operator< (const QTreeWidgetItem &other) const{
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
    void mousePressEvent(QMouseEvent *e);

private:
    Qt::SortOrder sort;
    QMenu *contextMenu;
signals:

private slots:
    void onHeaderClicked(int index);
    void onConnContext(QPoint p);

};

#endif // CUSTOMTREEWIDGET_H
