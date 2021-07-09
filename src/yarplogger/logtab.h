/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex>
#include <QFrame>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <yarp/logger/YarpLogger.h>
#include "messageWidget.h"
#include "logtabSorting.h"
#include "logmodel.h"

namespace Ui {
class LogTab;
}

class LogTab : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(LogTab)

public:
    explicit LogTab(yarp::yarpLogger::LoggerEngine*  _theLogger,
                    MessageWidget* _system_message,
                    std::string _portName,
                    QWidget *parent = 0,
                    int refreshRate = 100);
    ~LogTab();

private:
    Ui::LogTab*                     ui;
    std::string                     portName;
    yarp::yarpLogger::LoggerEngine* theLogger;
    MessageWidget*                  system_message;
    QMutex                          mutex;
    bool                            displayYarprunTimestamp_enabled;
    bool                            displayLocalTimestamp_enabled;
    bool                            displaySystemTime_enabled;
    bool                            displayNetworkTime_enabled;
    bool                            displayExternalTime_enabled;
    bool                            displayLogLevel_enabled;
    bool                            displayFilename_enabled;
    bool                            displayLine_enabled;
    bool                            displayFunction_enabled;
    bool                            displayHostname_enabled;
    bool                            displayPid_enabled;
    bool                            displayCmd_enabled;
    bool                            displayArgs_enabled;
    bool                            displayThreadId_enabled;
    bool                            displayComponent_enabled;
    bool                            displayColors_enabled;
    bool                            displayGrid_enabled;
    bool                            toggleLineExpansion;
    int                             sectionHeight;

private slots:
    void updateLog(bool from_beginning=false);
    void ctxMenu(const QPoint &pos);
    void expandLines();
    void on_copy_to_clipboard_action();

public:
    QTimer                  *logTimer;
    LogModel                *logModel;
#define USE_FILTERS 1
#if USE_FILTERS
    LogSortFilterProxyModel *proxyModelButtons;
    LogSortFilterProxyModel *proxyModelSearch;
#endif
    QClipboard              *clipboard;

    void displayYarprunTimestamp(bool enabled);
    void displayLocalTimestamp  (bool enabled);
    void displaySystemTime      (bool enabled);
    void displayNetworkTime     (bool enabled);
    void displayExternalTime    (bool enabled);
    void displayLogLevel        (bool enabled);
    void displayFilename        (bool enabled);
    void displayLine            (bool enabled);
    void displayFunction        (bool enabled);
    void displayHostname        (bool enabled);
    void displayPid             (bool enabled);
    void displayCmd             (bool enabled);
    void displayArgs            (bool enabled);
    void displayThreadId        (bool enabled);
    void displayComponent       (bool enabled);
    void displayColors          (bool enabled);
    void displayGrid            (bool enabled);

    void clearLogModel();
};

#endif // LOGTAB_H
