/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef CLUSTERWIDGET_H
#define CLUSTERWIDGET_H

#include <QWidget>
#include <tinyxml.h>
#include <vector>

namespace Ui {
class ClusterWidget;
}

struct ClusNode
{
    std::string name = "";
    bool display = false;
    std::string displayValue = "none";
    std::string user = "";
    std::string ssh_options = "";
    bool onOff = false;
    bool log = true;
};

struct Cluster
{
    std::string name = "";
    std::string user = "";
    std::string nameSpace = "";
    std::string nsNode = "";
    std::string ssh_options = "";
    std::vector<ClusNode> nodes;
};

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
public:
    explicit ClusterWidget(QWidget *parent = 0);
    ~ClusterWidget();

private:
    void addRow(std::string name="", std::string display="none", std::string user="",
                               bool onOff=false, bool log=true, bool select=true);
    std::string getSSHCmd(std::string user, std::string host, std::string ssh_options);
    bool parseConfigFile();
    bool checkNameserver();
    bool checkNode(std::string name);
private:
    Ui::ClusterWidget *ui;
    std::string confFile;
    Cluster cluster;
};

#endif // CLUSTERWIDGET_H
