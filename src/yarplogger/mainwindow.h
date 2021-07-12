/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <yarp/logger/YarpLogger.h>
#include <yarp/os/ResourceFinder.h>
#include "messageWidget.h"
#include "yarprunPortSorting.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_filterLineEdit_textChanged(const QString &text);

    void updateMain();

    void on_logtabs_tabCloseRequested(int index);

    void on_yarprunTreeView_doubleClicked(const QModelIndex &index);

    void on_DisplayErrorEnable_toggled(bool checked);

    void on_DisplayWarningEnable_toggled(bool checked);

    void on_DisplayDebugEnable_toggled(bool checked);

    void on_DisplayInfoEnable_toggled(bool checked);

    void on_DisplayTraceEnable_toggled(bool checked);

    void on_DisplayUnformattedEnable_toggled(bool checked);

    void on_actionAbout_QtYarpLogger_triggered();

    void on_actionStart_Logger_triggered();

    void ctxMenu(const QPoint &pos);

    void on_clearLogTab_action();

    void on_saveLogTab_action();

    void on_enableLogTab_action();

    void on_resetCountersLogTab_action();

    void on_actionSave_Log_triggered(bool checked);

    void on_actionLoad_Log_triggered();

    void on_actionShow_YarprunTimestamps_toggled(bool checked);

    void on_actionShow_LocalTimestamps_toggled(bool checked);

    void on_actionShow_System_Time_toggled(bool checked);

    void on_actionShow_Network_Time_toggled(bool checked);

    void on_actionShow_Custom_Time_toggled(bool checked);

    void on_actionShow_Log_Level_toggled(bool checked);

    void on_actionShow_Filename_toggled(bool checked);

    void on_actionShow_Line_Number_toggled(bool checked);

    void on_actionShow_Function_toggled(bool checked);

    void on_actionShow_Hostname_toggled(bool checked);

    void on_actionShow_Pid_toggled(bool checked);

    void on_actionShow_Cmd_toggled(bool checked);

    void on_actionShow_Args_toggled(bool checked);

    void on_actionShow_Thread_Id_toggled(bool checked);

    void on_actionShow_Component_toggled(bool checked);

    void on_actionShow_Colors_toggled(bool checked);

    void on_actionShow_Grid_toggled(bool checked);

    void on_actionShow_Mute_Ports_toggled(bool checked);

    void on_actionAdvanced_triggered();

    void on_actionStop_Logger_triggered();

    void on_actionRefresh_triggered();

    void on_actionClear_triggered();

    void on_actionClear_current_log_triggered();

    void on_actionExport_current_log_to_text_file_triggered();

    void on_actionDisable_current_log_triggered();

    void on_actionReset_current_log_error_warning_counters_triggered();

    void dragEnterEvent(QDragEnterEvent *e) override;

    void dropEvent(QDropEvent *e) override;

    void resetMainWindowHeaders();

private:
    yarp::yarpLogger::LoggerEngine* theLogger;

    Ui::MainWindow                   *ui;
    QStandardItemModel               *model_yarprunports;
    QItemSelectionModel              *selection_yarprunports;
    QTimer                           *mainTimer;
    MessageWidget                    *system_message;
    YarprunPortsSortFilterProxyModel *proxyModel;
    void loadTextFile();
    QString recomputeFilters();
    void apply_button_filters();

    //helper methods called by other slots
    void on_enableLogTab         (int model_row);
    void on_clearLogTab          (int model_row);
    void on_saveLogTab           (int model_row);
    void on_resetCountersLogTab  (int model_row);
};

#endif // MAINWINDOW_H
