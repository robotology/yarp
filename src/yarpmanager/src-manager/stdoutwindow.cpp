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
