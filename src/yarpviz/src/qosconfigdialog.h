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

#ifndef QOSCONFIGDIALOG_H
#define QOSCONFIGDIALOG_H

#include <QDialog>
#include <yarp/profiler/NetworkProfiler.h>
#include <yarp/profiler/Graph.h>
#include <qcombobox.h>
#include <qlineedit.h>

namespace Ui {
class QosConfigDialog;
}

class QosConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QosConfigDialog(yarp::profiler::graph::Edge *edge, QWidget *parent = 0);
    ~QosConfigDialog();
    bool getUserQosStyles(yarp::os::QosStyle& srcStyle, yarp::os::QosStyle& dstStyle);

private slots:
    void apply();
    void reset();

private:
    void createGui();

private:
    yarp::profiler::graph::Edge *edge;
    Ui::QosConfigDialog *ui;
    QComboBox* srcPacketProCombo;
    QLineEdit* srcThreadPro;
    QLineEdit* srcThreadPolicy;
    QComboBox* dstPacketProCombo;
    QLineEdit* dstThreadPro;
    QLineEdit* dstThreadPolicy;
};

#endif // QOSCONFIGDIALOG_H
