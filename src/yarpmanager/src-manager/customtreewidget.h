/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
