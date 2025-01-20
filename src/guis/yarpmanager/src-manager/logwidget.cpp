/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "logwidget.h"
#include <QFileDialog>
#include <QCoreApplication>

LogWidget::LogWidget(QWidget *parent) :
    QListWidget(parent),
    contextMenu(nullptr)
{
    //contextMenu = new QMenu(this);
    clearLogAction = new QAction("Clear Log",this);
    saveLogAction = new QAction("Save Log",this);

    clearLogAction->setIcon(QIcon(":/clear.svg"));
    saveLogAction->setIcon(QIcon(":/file-save.svg"));

    this->addAction(clearLogAction);
    this->addAction(saveLogAction);

    connect(clearLogAction,SIGNAL(triggered()),this,SLOT(onClearLog()));
    connect(saveLogAction,SIGNAL(triggered()),this,SLOT(onSaveLog()));
}

void LogWidget::onClearLog()
{
    clear();
}

void LogWidget::onSaveLog()
{
    QString logFileName = QFileDialog::getSaveFileName(this,"Save the Log",QDir::homePath());

    QFile f(logFileName);
    f.open(QIODevice::WriteOnly);

    for(int i=0; i<count(); i++){
        QString line = item(i)->text() + "\n";
        f.write(line.toLatin1().data());
    }

    f.flush();
    f.close();

}
