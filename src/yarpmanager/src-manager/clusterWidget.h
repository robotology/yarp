/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H

#include <QWidget>
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
signals:
    void logError(QString);
public:
    explicit ClusterWidget(QWidget *parent = 0);
    ~ClusterWidget();
    void setConfigFile(const std::string& _confFile);
    void init();

private:
    void addRow(const std::string& name="", const std::string& display="none",
                const std::string& user="", bool onOff=false, bool log=true, int id=0);
    std::string getSSHCmd(const std::string& user, const std::string& host, const std::string& ssh_options);
    bool checkNameserver();
    bool checkNode(const std::string& name);
private:
    Ui::ClusterWidget *ui;
    std::string confFile;
    yarp::manager::Cluster cluster;
    yarp::manager::XmlClusterLoader* clusLoader;
};

#endif // CLUSTERWIDGET_H
