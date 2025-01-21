/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "stdoutwindow.h"
#include "ui_stdoutwindow.h"

StdoutWindow::StdoutWindow(int id, QString title,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StdoutWindow)
{
    ui->setupUi(this);
    this->id = id;
    if(!title.isEmpty()){
        setWindowTitle(title);
    }else{
        setWindowTitle("STDOUT");
    }

    connect(this,SIGNAL(internalAddMessage(QString)),this,SLOT(onInternalAddMessage(QString)),Qt::QueuedConnection);
}

StdoutWindow::~StdoutWindow()
{
    delete ui;
}

void StdoutWindow::addMessage(QString text)
{
    // ui->plainTextEdit->appendPlainText(text);
    emit internalAddMessage(text);
}

int StdoutWindow::getId()
{
    return id;
}

void StdoutWindow::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event);
    emit closeStdOut(id);
}

void StdoutWindow::onInternalAddMessage(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}
