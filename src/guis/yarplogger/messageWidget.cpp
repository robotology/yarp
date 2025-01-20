/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "messageWidget.h"
#include <QFileDialog>
#include <QCoreApplication>
#include <QDateTime>

MessageWidget::MessageWidget(QWidget *parent) :
    QListWidget(parent),
    contextMenu(nullptr)
{
    //contextMenu = new QMenu(this);
    clearLogAction = new QAction("Clear Log",this);
    saveLogAction = new QAction("Save Log",this);

    //clearLogAction->setIcon(QIcon(":/images/clear.png"));
    //saveLogAction->setIcon(QIcon(":/images/Save-icon.png"));

    this->addAction(clearLogAction);
    this->addAction(saveLogAction);

    connect(clearLogAction,SIGNAL(triggered()),this,SLOT(onClearLog()));
    connect(saveLogAction,SIGNAL(triggered()),this,SLOT(onSaveLog()));
}

void MessageWidget::onClearLog()
{
    clear();
}

void MessageWidget::onSaveLog()
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

void MessageWidget::addMessage (QString text, int level)
{
    QString dateformat = " [dd/MM/yyyy hh:mm:ss] ";
    QDateTime currDate = QDateTime::currentDateTime();
    QString date_s = currDate.toString ( dateformat );

    if (level == 0)
    {
        QString message_to_add = QString ("[INFO] ") + date_s + text;
        addItem(message_to_add);
        setCurrentRow(this->count() - 1);
    }
    else if (level == 1)
    {
        QString message_to_add = QString ("[WAR] ") + date_s + text;
        addItem(message_to_add);
        item(this->count() - 1)->setBackground(QColor("#FFF6C8"));
        setCurrentRow(this->count() - 1);
    }
    else if (level == 2)
    {
        QString message_to_add = QString ("[ERR] ") + date_s + text;
        addItem(message_to_add);
        item(this->count() - 1)->setBackground(QColor("#F9CCCA"));
        setCurrentRow(this->count() - 1);
    }
}
