/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PORTLOGGERDIALOG_H
#define PORTLOGGERDIALOG_H

#include <QDialog>
#include <yarp/profiler/NetworkProfiler.h>
#include <QTimer>
#include <QElapsedTimer>

namespace Ui {
class PortLoggerDialog;
}

class PortLoggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortLoggerDialog(yarp::profiler::graph::Graph *graph, QWidget *parent = 0);
    ~PortLoggerDialog();

private slots:
    void addConnections();
    void removeConnections();
    void startStopLoggers();
    void MyTimerSlot();
    void setLogPath();
    void openCons();

private:
    bool saveLog(std::string filename, yarp::os::Bottle* samples);
private:
    Ui::PortLoggerDialog *ui;
    bool isStarted;
    yarp::profiler::graph::Graph *graph;
    QTimer *timer;
    QElapsedTimer logTime;
};

#endif // PORTLOGGERDIALOG_H
