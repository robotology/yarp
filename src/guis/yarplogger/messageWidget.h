/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QListWidget>
#include <QAction>
#include <QMenu>
#include <QString>

enum MessageWidgetLevel
{
    MESSAGE_LEVEL_ERROR   = 2,
    MESSAGE_LEVEL_WARNING = 1,
    MESSAGE_LEVEL_INFO    = 0
};

class MessageWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(QWidget *parent = 0);

private:
    QMenu *contextMenu;
    QAction *clearLogAction;
    QAction *saveLogAction;
signals:

private slots:
    void onClearLog();
    void onSaveLog();

public:
    void addMessage (QString text, int level=0);

};

#endif // MESSAGEWIDGET_H
