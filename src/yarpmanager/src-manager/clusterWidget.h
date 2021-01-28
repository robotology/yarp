/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <vector>
#include <yarp/manager/xmlclusterloader.h>
#include <customtreewidget.h>

namespace Ui {
class ClusterWidget;
}

class ClusterWidget : public QWidget
{
    Q_OBJECT
private slots:
    void onCheckAll();
    void onCheckServer();
    void onRunServer();
    void onStopServer();
    void onKillServer();
    void onRunSelected();
    void onStopSelected();
    void onKillSelected();
    void onExecute();
    void onNodeSelectionChanged();
    void onExecuteTextChanged();
signals:
    void logError(QString);
    void logMessage(QString);
public:
    explicit ClusterWidget(QWidget *parent = 0);
    ~ClusterWidget();
    void setConfigFile(const std::string& _confFile);
    void init();

private:
    void addRow(const std::string& name="", const std::string& display="none",
                const std::string& user="", const std::string& address="", bool onOff=false, bool log=true, int id=0);
    std::string getSSHCmd(const std::string& user, const std::string& host, const std::string& ssh_options);
    bool checkNameserver();
    bool checkNode(const std::string& name);
    void updateServerEntries();


    Ui::ClusterWidget *ui;
    std::string confFile;
    yarp::manager::Cluster cluster;
    yarp::manager::XmlClusterLoader* clusLoader;
    bool checkNs;
};

#endif // CLUSTERWIDGET_H
