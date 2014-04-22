/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
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

}

StdoutWindow::~StdoutWindow()
{
    delete ui;
}

void StdoutWindow::addMessage(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
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
