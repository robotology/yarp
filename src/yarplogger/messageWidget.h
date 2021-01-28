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
