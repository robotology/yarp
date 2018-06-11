/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef YSCOPEWINDOW_H
#define YSCOPEWINDOW_H

#include <QDialog>

namespace Ui {
class YscopeWindow;
}

class YscopeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit YscopeWindow(QWidget *parent = 0);
    ~YscopeWindow();
    int getIndex();

private:
    Ui::YscopeWindow *ui;
    int index;

private slots:
    void onInspect();
    void onCancel();
    void onIndexChanged(int);
};

#endif // YSCOPEWINDOW_H
