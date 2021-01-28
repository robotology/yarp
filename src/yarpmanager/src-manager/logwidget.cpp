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
