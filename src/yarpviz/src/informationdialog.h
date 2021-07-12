/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INFORMATIONDIALOG_H
#define INFORMATIONDIALOG_H

#include <QDialog>
#include <yarp/profiler/NetworkProfiler.h>
#include <yarp/profiler/Graph.h>

namespace Ui {
class InformationDialog;
}

class InformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InformationDialog(QWidget *parent = 0);
    ~InformationDialog();
    Ui::InformationDialog* getUi();

    void setProcessVertexInfo(yarp::profiler::graph::ProcessVertex* vertex);
    void setPortVertexInfo(yarp::profiler::graph::PortVertex* vertex);
    void setEdgeInfo(yarp::profiler::graph::Edge *edge);

private:
    Ui::InformationDialog *ui;
};

#endif // INFORMATIONDIALOG_H
