/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
