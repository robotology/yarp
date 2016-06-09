/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    internalAddMessage(text);
}

int StdoutWindow::getId()
{
    return id;
}

void StdoutWindow::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event);
    closeStdOut(id);
}

void StdoutWindow::onInternalAddMessage(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}
