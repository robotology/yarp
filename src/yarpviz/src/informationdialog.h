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
