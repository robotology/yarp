/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
