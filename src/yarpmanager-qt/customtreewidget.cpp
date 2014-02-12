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


void CustomTreeWidget::setContextMenu(QMenu *menu)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = menu;
}

void CustomTreeWidget::onConnContext(QPoint p)
{
    if(contextMenu){
        QPoint pp = QPoint(p.x(),p.y() + header()->height());
        contextMenu->exec(mapToGlobal(pp));
    }
}
