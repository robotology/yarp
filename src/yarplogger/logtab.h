/* 
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
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

#ifndef LOGTAB_H
#define LOGTAB_H

#include "LoggerViewModel.h"

#include <QMutex>
#include <QFrame>
#include <QTimer>
#include <QClipboard>
#include <yarp/logger/YarpLogger.h>
#include "messageWidget.h"

const std::string TRACE_STRING   = "TRACE";
const std::string DEBUG_STRING   = "DEBUG";
const std::string INFO_STRING    = "INFO";
const std::string WARNING_STRING = "WARNING";
const std::string ERROR_STRING   = "ERROR";
const std::string FATAL_STRING   = "FATAL";

class QSortFilterProxyModel;
class QIdentityProxyModel;

namespace Ui {
class LogTab;
}

class LogTab : public QFrame
{
    Q_OBJECT

public:
    explicit LogTab(yarp::yarpLogger::LoggerEngine*  _theLogger, MessageWidget* _system_message, std::string _portName, QWidget *parent = 0, int refreshRate=100);
    ~LogTab();

private:
    Ui::LogTab *ui;
    std::string                            portName;
    yarp::yarpLogger::LoggerEngine*        theLogger;
    MessageWidget*                         system_message;
    QMutex                                 mutex;
    bool                                   displayYarprunTimestamp_enabled;
    bool                                   displayLocalTimestamp_enabled;
    bool                                   displayErrorLevel_enabled;
    bool                                   displayColors_enabled;
    bool                                   displayGrid_enabled;
    bool                                   toggleLineExpansion;

private slots:
    void ctxMenu(const QPoint &pos);
    void expandLines();
    void on_copy_to_clipboard_action();

public:
    QClipboard              *clipboard;
    void                    displayYarprunTimestamp  (bool enabled);
    void                    displayLocalTimestamp    (bool enabled);
    void                    displayErrorLevel (bool enabled);
    void                    displayColors     (bool enabled);
    void                    displayGrid       (bool enabled);
    void                    clear_model_logs();


    void filterByLevel(QString levelString);
    void filterByMessage(QString message);

private:
    LoggerViewModel *logViewModel;
    QSortFilterProxyModel *logLevelFilterModelProxy;
    QSortFilterProxyModel *messageFilterModelProxy;

};

#endif // LOGTAB_H
