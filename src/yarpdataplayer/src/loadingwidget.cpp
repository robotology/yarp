/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
