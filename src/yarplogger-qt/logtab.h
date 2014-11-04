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

#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex>
#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <yarp/logger/YarpLogger.h>
#include "messageWidget.h"
#include "logtabSorting.h"

const std::string ERROR_STRING   = "ERROR";
const std::string WARNING_STRING = "WARNING";
const std::string INFO_STRING    = "INFO";
const std::string DEBUG_STRING   = "DEBUG";
const std::string TRACE_STRING   = "TRACE";

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

private slots:
    void updateLog(bool from_beginning=false);

public:
    QTimer                  *logTimer;
    QStandardItemModel      *model_logs;
    LogSortFilterProxyModel *proxyModelButtons;
    LogSortFilterProxyModel *proxyModelSearch;
    void                    displayYarprunTimestamp  (bool enabled);
    void                    displayLocalTimestamp    (bool enabled);
    void                    displayErrorLevel (bool enabled);
    void                    displayColors     (bool enabled);
    void                    displayGrid       (bool enabled);
    void                    clear_model_logs();
};

#endif // LOGTAB_H
