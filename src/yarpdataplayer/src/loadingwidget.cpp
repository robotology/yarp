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

#include "include/loadingwidget.h"
#include "ui_loadingwidget.h"

LoadingWidget::LoadingWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadingWidget),
    counter(0)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    splashTimer.setInterval(50);
    splashTimer.setSingleShot(false);
    connect(&splashTimer,SIGNAL(timeout()),this,SLOT(onSplashTimer()),Qt::DirectConnection);
}

LoadingWidget::~LoadingWidget()
{
    delete ui;
}


void LoadingWidget::onSplashTimer()
{
    QString name = QString(":/images/splash/bg%1.png").arg(counter+1);
    counter++;
    counter = counter % 8;
    ui->loadingIco->setPixmap(QPixmap(name));
    ui->loadingIco->repaint();
}


int LoadingWidget::start()
{
    QWidget *w  =((QWidget*)parent());
    int x = w->x() + ((w->width() - width()) / 2);
    int y = w->y() + ((w->height() - height()) / 2);
    this->setGeometry(x,y,width(),height());
    splashTimer.start();
    return exec();
}

void LoadingWidget::stop()
{
    splashTimer.stop();
}
