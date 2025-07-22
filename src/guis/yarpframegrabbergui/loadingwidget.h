/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LOADINGWIDGET_H
#define LOADINGWIDGET_H

#include <QDialog>
#include <QTimer>
#include <QMutex>
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
    QMutex mutex;


private slots:
    void onSplashTimer();
};

#endif // LOADINGWIDGET_H
