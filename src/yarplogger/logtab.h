/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex>
#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <yarp/logger/YarpLogger.h>
#include "messageWidget.h"
#include "logtabSorting.h"

const std::string TRACE_STRING   = "TRACE";
const std::string DEBUG_STRING   = "DEBUG";
const std::string INFO_STRING    = "INFO";
const std::string WARNING_STRING = "WARNING";
const std::string ERROR_STRING   = "ERROR";
const std::string FATAL_STRING   = "FATAL";

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
    yarp::yarpLogger::LoggerEngine* theLogger;
    MessageWidget*                         system_message;
    QMutex                                 mutex;
    bool                                   displayYarprunTimestamp_enabled;
    bool                                   displayLocalTimestamp_enabled;
    bool                                   displayErrorLevel_enabled;
    bool                                   displayColors_enabled;
    bool                                   displayGrid_enabled;
    bool                                   toggleLineExpansion;

private slots:
    void updateLog(bool from_beginning=false);
    void ctxMenu(const QPoint &pos);
    void expandLines();
    void on_copy_to_clipboard_action();

public:
    QTimer                  *logTimer;
    QStandardItemModel      *model_logs;
    LogSortFilterProxyModel *proxyModelButtons;
    LogSortFilterProxyModel *proxyModelSearch;
    QClipboard              *clipboard;
    void                    displayYarprunTimestamp  (bool enabled);
    void                    displayLocalTimestamp    (bool enabled);
    void                    displayErrorLevel (bool enabled);
    void                    displayColors     (bool enabled);
    void                    displayGrid       (bool enabled);
    void                    clear_model_logs();
};

#endif // LOGTAB_H
