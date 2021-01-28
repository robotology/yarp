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
