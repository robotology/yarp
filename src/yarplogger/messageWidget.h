/* 
 * Copyright (C)2014  iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
