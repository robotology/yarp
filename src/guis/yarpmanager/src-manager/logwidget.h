/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
