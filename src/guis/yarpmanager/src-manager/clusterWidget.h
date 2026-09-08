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

class QGroupBox;
class QPushButton;
class QTreeWidget;

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
    void onRefreshDockerContainers();
    void onStartSelectedDockerContainers();
    void onStopSelectedDockerContainers();
    void updateDockerButtons();
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
                const std::string& user="", const std::string& address="", const std::string& runtime="",
                const std::string& target="", bool onOff=false, bool log=true, int id=0);
    void addDockerControls();
    std::string getSSHCmd(const std::string& user, const std::string& host, const std::string& ssh_options, bool background = true);
    std::string buildYarprunCommand(const yarp::manager::ClusterNode& node, const std::string& portName, bool log, const std::string& logFile);
    bool runRemoteCommand(const yarp::manager::ClusterNode& node, const std::string& command, std::string& output);
    bool checkPixiRuntime(const yarp::manager::ClusterNode& node);
    std::vector<yarp::manager::DockerContainer> getDockerContainers(const yarp::manager::ClusterNode& node, const std::string& dockerName);
    bool runDockerCommand(const yarp::manager::DockerContainer& container, const std::string& command);
    int getNodeIndex(QTreeWidgetItem* item) const;
    bool isDockerRunning(const yarp::manager::ClusterNode& node, const std::string& dockerName);
    void updateYarprunDockerColors();
    bool checkNameserver();
    bool checkNode(const std::string& name);
    void updateServerEntries();
    void reportErrors();


    Ui::ClusterWidget *ui;
    std::string confFile;
    yarp::manager::Cluster cluster;
    yarp::manager::XmlClusterLoader* clusLoader;
    bool checkNs;
    QGroupBox* dockerGroupBox;
    QTreeWidget* dockerTreeWidget;
    QPushButton* dockerRefreshBtn;
    QPushButton* dockerStartBtn;
    QPushButton* dockerStopBtn;
};

#endif // CLUSTERWIDGET_H
