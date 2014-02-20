/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "customtreewidget.h"
#include <QMouseEvent>
#include <QHeaderView>
CustomTreeWidget::CustomTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onConnContext(QPoint)));
    connect(header(),SIGNAL(sectionClicked(int)),this,SLOT(onHeaderClicked(int)));
    setSortingEnabled(true);
    sort = Qt::AscendingOrder;
    sortByColumn(1,sort);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    contextMenu = NULL;
}

/*! \brief the mouse press event
    \param e the event
*/
void CustomTreeWidget::mousePressEvent(QMouseEvent *e)
{
    QTreeWidget::mousePressEvent(e);

    if(e->button() == Qt::RightButton){
        if(itemAt(e->pos()) == NULL){
            setCurrentItem(NULL);
        }
        if(currentItem() == NULL){
            itemSelectionChanged();
        }
    }
}

/*! \brief the press event on an header
    \param index index of the header has been pressed
*/
void CustomTreeWidget::onHeaderClicked(int index)
{
    if(index == sortColumn()){
        if(sort == Qt::AscendingOrder){
            sort = Qt::DescendingOrder;
        }else{
            sort = Qt::AscendingOrder;
        }
    }else{
        sort = Qt::AscendingOrder;
    }

    sortByColumn(index,sort);
}

/*! \brief Sets a context menu to the tree
    \param menu the context menu
*/
void CustomTreeWidget::setContextMenu(QMenu *menu)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = menu;
}

/*! \brief Called when a context menu hasb been requested
    \param p the point where the context menu should appear
*/
void CustomTreeWidget::onConnContext(QPoint p)
{
    if(contextMenu){
        QPoint pp = QPoint(p.x(),p.y() + header()->height());
        contextMenu->exec(mapToGlobal(pp));
    }
}
