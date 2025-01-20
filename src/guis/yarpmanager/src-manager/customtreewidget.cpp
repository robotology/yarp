/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
    contextMenu = nullptr;
}

/*! \brief the mouse press event
    \param e the event
*/
void CustomTreeWidget::mousePressEvent(QMouseEvent *e)
{


    if(e->button() == Qt::RightButton){
        if(itemAt(e->pos()) == nullptr){
            setCurrentItem(nullptr);
        }
        if(currentItem() == nullptr){
            emit itemSelectionChanged();
        }
    }
    QTreeWidget::mousePressEvent(e);
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
