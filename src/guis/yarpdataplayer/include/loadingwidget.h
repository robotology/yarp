/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QDialog>
#include <QTimer>
namespace Ui {
class LoadingWidget;
}

class LoadingWidget : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingWidget(QWidget *parent = 0);
    ~LoadingWidget();

    int start();
    void stop();

private:
    Ui::LoadingWidget *ui;
    QTimer splashTimer;
    int counter;


private slots:
    void onSplashTimer();
};

#endif // LOADINGWIDGET_H
