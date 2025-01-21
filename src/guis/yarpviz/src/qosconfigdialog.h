/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
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
