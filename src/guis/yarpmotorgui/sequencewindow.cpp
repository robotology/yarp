/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "sequencewindow.h"
#include "ui_sequencewindow.h"
#include <QMenu>
#include <QDebug>
#include <QClipboard>


SequenceWindow::SequenceWindow(QString partName, int count,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SequenceWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString("Sequence Window for part: %1").arg(partName));
    goAction        = ui->toolBar->addAction(QIcon(":/play.svg"),"Run (use joint speeds from Speed tab)");
    runTimeAction    = ui->toolBar->addAction(QIcon(":/images/runSequenceTime.png"),"Run (ignore Speed tab, produce coordinated movement using Timing)");

    saveAction       = ui->toolBar->addAction(QIcon(":/file-save.svg"),"Save");
    openAction       = ui->toolBar->addAction(QIcon(":/file-open.svg"),"Open");
    cycleAllAction   = ui->toolBar->addAction(QIcon(":/images/cycleAllSequence.png"),"Cycle (use joint speeds from Speed tab)");
    cycleTimeAction  = ui->toolBar->addAction(QIcon(":/images/cycleAllSequenceTime.png"),"Cycle (ignore Speed tab, produce coordinated movement using Timing)");
    stopAction       = ui->toolBar->addAction(QIcon(":/stop.svg"),"Stop");

    QMenu *menu = ui->menuBar->addMenu("Commands");
    menu->addAction(goAction);
    menu->addAction(runTimeAction);
    menu->addAction(saveAction);
    menu->addAction(openAction);
    menu->addAction(cycleAllAction);
    menu->addAction(cycleTimeAction);
    menu->addAction(stopAction);

    connect(goAction,SIGNAL(triggered()),this,SLOT(onGo()));
    connect(runTimeAction,SIGNAL(triggered()),this,SLOT(onRunTime()));
    connect(saveAction,SIGNAL(triggered()),this,SLOT(onSave()));
    connect(openAction,SIGNAL(triggered()),this,SLOT(onOpen()));
    connect(cycleAllAction,SIGNAL(triggered()),this,SLOT(onCycle()));
    connect(cycleTimeAction,SIGNAL(triggered()),this,SLOT(onCycleTime()));
    connect(stopAction,SIGNAL(triggered()),this,SLOT(onStopSequence()));

    jointCount = count;
    prevCurrentIndex = -1;

    QStringList headers;
    headers << "Sequence" << "Timing";

    QStringList headers1;
    headers1 << "Sequence";

    for(int i=0;i<jointCount;i++){
        headers << QString("Joint %1").arg(i);
        headers1 << QString("Joint %1").arg(i);
    }


    ui->treePositions->setHeaderLabels(headers);
    ui->treeSpeed->setHeaderLabels(headers1);

    connect(ui->treePositions,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(onDoubleClickPositions(QTreeWidgetItem*,int)));

    connect(ui->treeSpeed,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(onDoubleClickSpeed(QTreeWidgetItem*,int)));

    connect(ui->treePositions,SIGNAL(deletedItem(int)),this,SLOT(onDeletePosition(int)));
    connect(ui->treePositions,SIGNAL(moveItem(int,int)),this,SLOT(onMovePositions(int,int)));

    QStringList ss;
    ss << "0" << "-0.1";

    QStringList ss1;
    ss1 << "0";

    for(int i=0; i<jointCount;i++){
        ss << "0";
        ss1 << "10";
    }
    auto* itemPos = new QTreeWidgetItem(ss);
    ui->treePositions->addTopLevelItem(itemPos);

    auto* itemSpeed = new QTreeWidgetItem(ss1);
    ui->treeSpeed->addTopLevelItem(itemSpeed);
}

SequenceWindow::~SequenceWindow()
{
    delete ui;
}


void SequenceWindow::onMovePositions(int index, int dragIndex)
{
    QTreeWidgetItem * it = ui->treeSpeed->takeTopLevelItem(dragIndex);

    ui->treeSpeed->insertTopLevelItem(index ,it);


    // Re-Assign sequential id
    for(int i=0;i<ui->treeSpeed->topLevelItemCount();i++){
        ui->treeSpeed->topLevelItem(i)->setText(0,QString("%1").arg(i));
    }
}

void SequenceWindow::onDeletePosition(int index)
{
    delete ui->treeSpeed->takeTopLevelItem(index);

    // Re-Assign sequential id
    for(int i=0;i<ui->treeSpeed->topLevelItemCount();i++){
        ui->treeSpeed->topLevelItem(i)->setText(0,QString("%1").arg(i));
    }
}

void SequenceWindow::closeEvent(QCloseEvent *event)
{
    if(!isVisible()){
        QMainWindow::closeEvent(event);
        return;
    }

    event->ignore();
    hide();

}


void SequenceWindow::onDoubleClickSpeed(QTreeWidgetItem *item,int column)
{

    if(column > 0){
        //ui->treePositions->openPersistentEditor(item,column);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        return;
    }else{
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }



}



void SequenceWindow::onDoubleClickPositions(QTreeWidgetItem *item,int column)
{

    if(column >= 1){
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        return;
    }else{
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }


    emit itemDoubleClicked(item->text(0).toInt());

    if(item != ui->treePositions->topLevelItem(ui->treePositions->topLevelItemCount()-1)){
        return;
    }



    QStringList ss;
    ss << QString("%1").arg(ui->treePositions->topLevelItemCount()) << "-0.1";

    QStringList ss1;
    ss1 << QString("%1").arg(ui->treePositions->topLevelItemCount());

    for(int i=0; i<jointCount;i++){
        ss << "0";
    }
    auto* newItem = new QTreeWidgetItem(ss);
    item->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
    ui->treePositions->addTopLevelItem(newItem);

    auto* newItem1 = new QTreeWidgetItem(ss1);
    ui->treeSpeed->addTopLevelItem(newItem1);



}



void SequenceWindow::onReceiveValues(int sequenceNum, QList<double> values,QList<double> speeds)
{
    for(int i=0;i<values.count();i++){
        ui->treePositions->topLevelItem(sequenceNum)->setText(i+2,QString("%1").arg(values.at(i)));
        //ui->treePositions->topLevelItem(sequenceNum)->setData(i+2,Qt::UserRole,speeds.at(i));
        ui->treeSpeed->topLevelItem(sequenceNum)->setText(i+1,QString("%1").arg(speeds.at(i)));
    }
}


bool SequenceWindow::checkAndGo()
{
    if(ui->treePositions->currentItem() == nullptr){
        return false;
    }
    int index = ui->treePositions->currentIndex().row();
    double timing = ui->treePositions->topLevelItem(index)->text(1).toDouble();

    if(timing <= 0){
        return false;
    }

    onGo();
    return true;
}

void SequenceWindow::onGo()
{
    double timing = -0.1;

    SequenceItem sequenceItem;

    if(ui->treePositions->currentItem() != nullptr){
        int index = ui->treePositions->currentIndex().row();
        timing = ui->treePositions->topLevelItem(index)->text(1).toDouble();

        sequenceItem.setSequenceNumber(index);
        sequenceItem.setTiming(timing);

        for(int i=0;i<ui->treePositions->columnCount() - 2;i++){
            double pos   = ui->treePositions->topLevelItem(index)->text(i+2).toDouble();
            double speed = ui->treeSpeed->topLevelItem(index)->text(i+1).toDouble();
            //double speed = ui->treePositions->topLevelItem(index)->data(i+2,Qt::UserRole).toDouble();

            sequenceItem.addValue(pos,speed);

        }
    }

    emit goToPosition(sequenceItem);
}

void SequenceWindow::onRunTime()
{
    emit runTime(getValuesFromList());
}

bool SequenceWindow::checkAndCycleTimeSeq()
{
    QList <SequenceItem> seq = getValuesFromList();
    if(seq.count() <= 0){
        return false;
    }

    int count = 0;
    for(int i=0;i<seq.count();i++){
        SequenceItem item = seq.at(i);
        if(item.getTiming() > 0){
            count++;
        }else{
            break;
        }
    }
    if(count <= 1){
        return false;
    }
    emit cycleTime(seq);
    return true;
}

bool SequenceWindow::checkAndCycleSeq()
{
    QList <SequenceItem> seq = getValuesFromList();
    if(seq.count() <= 0){
        return false;
    }

    int count = 0;
    for(int i=0;i<seq.count();i++){
        SequenceItem item = seq.at(i);
        if(item.getTiming() > 0){
            count++;
        }else{
            break;
        }
    }
    if(count <= 1){
        return false;
    }
    emit cycle(seq);
    return true;
}

bool SequenceWindow::checkAndRun()
{
    QList <SequenceItem> seq = getValuesFromList();
    if(seq.count() <= 0){
        return false;
    }

    int count = 0;
    for(int i=0;i<seq.count();i++){
        SequenceItem item = seq.at(i);
        if(item.getTiming() > 0){
            count++;
        }else{
            break;
        }
    }
    if(count <= 1){
        return false;
    }
    emit run(seq);
    return true;

}

bool SequenceWindow::checkAndRunTime()
{
    QList <SequenceItem> seq = getValuesFromList();
    if(seq.count() <= 0){
        return false;
    }

    int count = 0;
    for(int i=0;i<seq.count();i++){
        SequenceItem item = seq.at(i);
        if(item.getTiming() > 0){
            count++;
        }else{
            break;
        }
    }
    if(count <= 1){
        return false;
    }
    emit runTime(seq);
    return true;

}

void SequenceWindow::onCycle()
{
    emit cycle(getValuesFromList());
}

void SequenceWindow::onStopSequence()
{
    emit stopSequence();
}

void SequenceWindow::onCycleTime()
{
    emit cycleTime(getValuesFromList());
}

void SequenceWindow::onOpen()
{
    emit openSequence();
}

void SequenceWindow::save(QString global_filename)
{
    emit saveSequence(getValuesFromList(),global_filename);
}


void SequenceWindow::onSave()
{
    save("");
}

void SequenceWindow::loadSequence(QList<SequenceItem> sequence)
{
    ui->treePositions->clear();

    QStringList ss;
    QStringList ss1;

    ui->treePositions->clear();
    ui->treeSpeed->clear();

    for(int i=0; i<sequence.count();i++){
        SequenceItem seq = sequence.at(i);

        ss.append(QString("%1").arg(seq.getSequenceNumber()));
        ss.append(QString("%1").arg(seq.getTiming()));

        ss1.append(QString("%1").arg(seq.getSequenceNumber()));


        for(int j=0;j<seq.getPositions().count();j++){
            ss.append(QString("%1").arg(seq.getPositions().at(j)));
        }

        for(int i=0;i<seq.getSpeeds().count();i++){
            ss1.append(QString("%1").arg(seq.getSpeeds().at(i)));
        }

        auto* newItem = new QTreeWidgetItem(ss);
        ui->treePositions->addTopLevelItem(newItem);


        auto* newItem1 = new QTreeWidgetItem(ss1);
        ui->treeSpeed->addTopLevelItem(newItem1);

        ss.clear();
        ss1.clear();
    }
}

QList <SequenceItem> SequenceWindow::getValuesFromList()
{
    QList <SequenceItem> valuesList;

    for(int i=0;i<ui->treePositions->topLevelItemCount();i++){
        QTreeWidgetItem *item = ui->treePositions->topLevelItem(i);
        QTreeWidgetItem *item1 = ui->treeSpeed->topLevelItem(i);

        double timing = item->text(1).toDouble();

        SequenceItem sequenceitem;
        sequenceitem.setSequenceNumber(i);
        sequenceitem.setTiming(timing);

        for(int j=2;j<item->columnCount();j++){
            sequenceitem.addValue(item->text(j).toDouble(),item1->text(j-1).toDouble());
        }
        valuesList.append(sequenceitem);
    }
    return valuesList;
}

void SequenceWindow::onRunTimeSequence()
{
    runTimeAction->setEnabled(true);
    cycleAllAction->setEnabled(false);
    cycleTimeAction->setEnabled(false);
    goAction->setEnabled(false);
    saveAction->setEnabled(false);
    openAction->setEnabled(false);

}

void SequenceWindow::onCycleTimeSequence()
{
    runTimeAction->setEnabled(false);
    cycleAllAction->setEnabled(false);
    cycleTimeAction->setEnabled(true);
    goAction->setEnabled(false);
    saveAction->setEnabled(false);
    openAction->setEnabled(false);

}

void SequenceWindow::onCycleSequence()
{
    runTimeAction->setEnabled(false);
    cycleAllAction->setEnabled(true);
    cycleTimeAction->setEnabled(false);
    goAction->setEnabled(false);
    saveAction->setEnabled(false);
    openAction->setEnabled(false);
}

void SequenceWindow::onStoppedSequence()
{
    runTimeAction->setEnabled(true);
    cycleAllAction->setEnabled(true);
    cycleTimeAction->setEnabled(true);
    goAction->setEnabled(true);
    saveAction->setEnabled(true);
    openAction->setEnabled(true);


    if(prevCurrentIndex >=0){
        for(int i=0;i<ui->treePositions->columnCount();i++){
            ui->treePositions->topLevelItem(prevCurrentIndex)->setBackground(i,QColor(0,0,0,0));
        }
        ui->treePositions->topLevelItem(prevCurrentIndex)->setIcon(0,QIcon());

        for(int i=0;i<ui->treeSpeed->columnCount();i++){
            ui->treeSpeed->topLevelItem(prevCurrentIndex)->setBackground(i,QColor(0,0,0,0));
        }
        ui->treeSpeed->topLevelItem(prevCurrentIndex)->setIcon(0,QIcon());

        prevCurrentIndex = -1;

    }
}

void SequenceWindow::onSetCurrentSequenceIndex(int index)
{
    if(prevCurrentIndex >=0){
        for(int i=0;i<ui->treePositions->columnCount();i++){
            ui->treePositions->topLevelItem(prevCurrentIndex)->setBackground(i,QColor(0,0,0,0));
        }
        ui->treePositions->topLevelItem(prevCurrentIndex)->setIcon(0,QIcon());

        for(int i=0;i<ui->treeSpeed->columnCount();i++){
            ui->treeSpeed->topLevelItem(prevCurrentIndex)->setBackground(i,QColor(0,0,0,0));
        }
        ui->treeSpeed->topLevelItem(prevCurrentIndex)->setIcon(0,QIcon());


    }
    for(int i=0;i<ui->treePositions->columnCount();i++){
        ui->treePositions->topLevelItem(index)->setBackground(i,QColor(0,255,0,120));

    }
    ui->treePositions->topLevelItem(index)->setIcon(0,QIcon(":/play.svg"));

    for(int i=0;i<ui->treeSpeed->columnCount();i++){
        ui->treeSpeed->topLevelItem(index)->setBackground(i,QColor(0,255,0,120));

    }
    ui->treeSpeed->topLevelItem(index)->setIcon(0,QIcon(":/play.svg"));
    prevCurrentIndex = index;
}



/**************************************************************************************/
SequenceTreeWidget::SequenceTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onContextMenuRequested(QPoint)));
}

void SequenceTreeWidget::dropEvent(QDropEvent *event)
{
    event->ignore();
    QTreeWidgetItem *item = itemAt(event->pos());
    QList<QTreeWidgetItem*> dragItems = selectedItems();

    if(!item || dragItems.isEmpty()){
        return;
    }
    int index = indexOfTopLevelItem(item);

    QTreeWidgetItem *dragIt = dragItems.at(0);
    int dragIndex = indexOfTopLevelItem(dragIt);
    QTreeWidgetItem * it = takeTopLevelItem(dragIndex);

    insertTopLevelItem(index ,it);

    emit moveItem(index,dragIndex);


    // Re-Assign sequential id
    for(int i=0;i<topLevelItemCount();i++){
        topLevelItem(i)->setText(0,QString("%1").arg(i));
    }
}

void SequenceTreeWidget::onContextMenuRequested(QPoint point)
{
    QPoint p = point;
    QTreeWidgetItem *item = itemAt(p);

    if(!item){
        return;
    }

    QMenu menu;
    QAction *copyAction = menu.addAction("Copy row");
    QAction *pasteAction = menu.addAction("Paste row");
    QAction *deleteAction = menu.addAction("Delete Row");
    QAction *clipboardAction = menu.addAction("Copy to clipboard");

    if(copyValues.isEmpty()){
        pasteAction->setEnabled(false);
    }

    p.setY(point.y() + header()->height());
    QAction *ret = menu.exec(mapToGlobal(p));

    if (ret == clipboardAction)
    {
        QClipboard  *clipboard = QApplication::clipboard();

        QString selected_test;

        for (int i = 2; i<columnCount(); i++)
        {
            selected_test = selected_test + item->text(i);
            selected_test = selected_test + QString("\t");
        }
        clipboard->setText(selected_test);
    }

    if(ret == copyAction)
    {
        copyValues.clear();
        for(int i=1; i<columnCount(); i++){
            copyValues.append(item->text(i));
        }
    }

    if(ret == pasteAction){
        for(int i=1; i<columnCount(); i++){
            item->setText(i,copyValues.at(i-1));
        }
    }

    if(ret == deleteAction){
        QList<QTreeWidgetItem*> deleteItems = selectedItems();
        if(deleteItems.isEmpty() || topLevelItemCount() <= 1){
            return;
        }

        int index = indexOfTopLevelItem(deleteItems.at(0));
        delete takeTopLevelItem(index);

        emit deletedItem(index);
        // Re-Assign sequential id
        for(int i=0;i<topLevelItemCount();i++){
            topLevelItem(i)->setText(0,QString("%1").arg(i));
        }
    }
}
