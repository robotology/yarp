/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QListWidget>
#include <QAction>
#include <QMenu>
class LogWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit LogWidget(QWidget *parent = 0);

private:
    QMenu *contextMenu;
    QAction *clearLogAction;
    QAction *saveLogAction;
signals:

private slots:
    void onClearLog();
    void onSaveLog();

};

#endif // LOGWIDGET_H
