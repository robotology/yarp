/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "clusterWidget.h"
#include "ui_clusterWidget.h"
#include "iostream"
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QStringList>

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/NameClient.h>

#include <mainwindow.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::manager;

ClusterWidget::ClusterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterWidget), confFile(""), clusLoader(YARP_NULLPTR)
{

#ifdef WIN32
    this->setDisabled(true);
    return;
#endif
    ui->setupUi(this);

    ui->checkNs->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkNs->setFocusPolicy(Qt::NoFocus);

    ui->checkNs->setStyleSheet("QCheckBox { color: green }");

    //Connections to slots

    //nameserver
    connect(ui->checkServerBtn, SIGNAL(clicked(bool)), this, SLOT(onCheckServer()));
    connect(ui->runServerBtn, SIGNAL(clicked(bool)), this, SLOT(onRunServer()));
    connect(ui->stopServerBtn, SIGNAL(clicked(bool)), this, SLOT(onStopServer()));
    //yarprun
    connect(ui->checkAllBtn, SIGNAL(clicked(bool)), this, SLOT(onCheckAll()));
    connect(ui->runSelBtn, SIGNAL(clicked(bool)), this, SLOT(onRunSelected()));
    connect(ui->stopSelBtn, SIGNAL(clicked(bool)), this, SLOT(onStopSelected()));
    connect(ui->killSelBtn, SIGNAL(clicked(bool)), this, SLOT(onKillSelected()));
    //execute
    connect(ui->executeBtn, SIGNAL(clicked(bool)), this, SLOT(onExecute()));

    connect(ui->nodestreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onNodeSelectionChanged()));

}

void ClusterWidget::setConfigFile(const string &_confFile)
{
    confFile = _confFile;
}

void ClusterWidget::init()
{
    clusLoader = new XmlClusterLoader(confFile);
    if (clusLoader)
    {
        if (!clusLoader->parseXmlFile(cluster))
        {
            yError()<<"ClusterWidget:Unable parse cluster-config.xml in context iCubCluster";
            this->setDisabled(true);
            return;
        }
    }

    ui->lineEditUser->setText(cluster.user.c_str());
    ui->lineEditNs->setText(cluster.nameSpace.c_str());
    ui->lineEditNsNode->setText(cluster.nsNode.c_str());

    //check if yarpserver is running

    onCheckServer();

    //Adding nodes

    for (size_t i = 0; i<cluster.nodes.size(); i++)
    {
        ClusterNode node = cluster.nodes[i];
        addRow(node.name, node.displayValue, node.user, node.onOff, node.log, i);
    }

    //check if all the nodes are up
    if (ui->checkNs->isChecked())
    {
        onCheckAll();
    }

    ui->nodestreeWidget->header()->resizeSection(0, 50);

    onNodeSelectionChanged();

}

void ClusterWidget::onCheckAll()
{
    for (int i = 0; i<ui->nodestreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *it = ui->nodestreeWidget->topLevelItem(i);
        int itr = it->text(5).toInt();
        ClusterNode node = cluster.nodes[itr];
        if (checkNode(node.name))
        {
            cluster.nodes[itr].onOff=true;
            it->setIcon(0, QIcon(":/computer-available22.svg"));
        }
        else
        {
            cluster.nodes[itr].onOff=false;
            it->setIcon(0, QIcon(":/computer-unavailable22.svg"));
        }
    }

}

void ClusterWidget::onCheckServer()
{
    ui->checkNs->setChecked(checkNameserver());
}

void ClusterWidget::onRunServer()
{
    string cmdRunServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);
    if (ui->checkRos->isChecked())
    {
        cmdRunServer = cmdRunServer + " yarpserver --portdb :memory: --subdb :memory: --ros >/dev/null 2>&1 &";
    }
    else
    {
        cmdRunServer = cmdRunServer + " yarpserver --portdb :memory: --subdb :memory: >/dev/null 2>&1 &";
    }
    if (system(cmdRunServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to run the server on" + cluster.nsNode;
        logError(QString(err.c_str()));
    }
    else
    {
        yarp::os::Time::delay(1.0);
        onCheckServer();
    }

}

void ClusterWidget::onStopServer()
{
    string cmdStopServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);

    cmdStopServer = cmdStopServer + " killall yarpserver";

    if (system(cmdStopServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to stop the server on" + cluster.nsNode;
        logError(QString(err.c_str()));
    }
    else
    {
        yarp::os::Time::delay(1.0);
        onCheckServer();
    }

    // if it fails to stop, kill it
    if (ui->checkNs->isChecked())
    {
        onKillServer();
    }
}

void ClusterWidget::onKillServer()
{
    string cmdKillServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);

    cmdKillServer = cmdKillServer + " killall -9 yarpserver";

    if (system(cmdKillServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to kill the server on" + cluster.nsNode;
        logError(QString(err.c_str()));
    }


}

void ClusterWidget::onRunSelected()
{
    QList<QTreeWidgetItem*> selectedItems = ui->nodestreeWidget->selectedItems();
    foreach (QTreeWidgetItem *it, selectedItems)
    {
        int itr = it->text(5).toInt();
        ClusterNode node = cluster.nodes[itr];
        string portName = node.name;

        if (portName.find("/") == std::string::npos)
        {
            portName = "/" + portName;
        }

        if (node.onOff)
        {
            continue;
        }

        string cmdRunYarprun = getSSHCmd(node.user, node.name, node.ssh_options);
        if (node.display)
        {
            cmdRunYarprun = cmdRunYarprun + " 'export DISPLAY=" + node.displayValue + " && ";

        }
        if (qobject_cast<QCheckBox*>(ui->nodestreeWidget->itemWidget((QTreeWidgetItem *)it, 4))->isChecked())
        {
            cmdRunYarprun = cmdRunYarprun + " yarprun --server "+ portName  + " --log 2>&1 2>/tmp/yarprunserver.log";
        }
        else
        {
            cmdRunYarprun = cmdRunYarprun + " yarprun --server "+ portName  + " 2>&1 2>/tmp/yarprunserver.log";
        }

        if (node.display)
        {
            cmdRunYarprun = cmdRunYarprun + "'";
        }
        if (system(cmdRunYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to run yarprun on" + node.name;
            logError(QString(err.c_str()));
        }
    }

    yarp::os::Time::delay(2.0);
    onCheckAll();
}


void ClusterWidget::onStopSelected()
{
    QList<QTreeWidgetItem*> selectedItems = ui->nodestreeWidget->selectedItems();
    foreach (QTreeWidgetItem *it, selectedItems)
    {
        int itr = it->text(5).toInt();
        ClusterNode node = cluster.nodes[itr];
        if (!node.onOff)
        {
            continue;
        }
        string portName = node.name;
        if (portName.find("/") == std::string::npos)
        {
            portName = "/" + portName;
        }

        string cmdStopYarprun = getSSHCmd(node.user, node.name, node.ssh_options);

        cmdStopYarprun = cmdStopYarprun + " yarprun --exit --on "+ portName;

        if (system(cmdStopYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to stop yarprun on" + node.name;
            logError(QString(err.c_str()));
        }
    }

    yarp::os::Time::delay(2.0);
    onCheckAll();
}

void ClusterWidget::onKillSelected()
{
    QList<QTreeWidgetItem*> selectedItems = ui->nodestreeWidget->selectedItems();
    foreach (QTreeWidgetItem *it, selectedItems)
    {
        int itr = it->text(5).toInt();
        ClusterNode node = cluster.nodes[itr];
        if (!node.onOff)
        {
            continue;
        }

        string cmdKillYarprun = getSSHCmd(node.user, node.name, node.ssh_options);

        cmdKillYarprun = cmdKillYarprun + " killall -9 yarprun";

        if (system(cmdKillYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to kill yarprun on" + node.name;
            logError(QString(err.c_str()));
        }
    }
    yarp::os::Time::delay(2.0);
    onCheckAll();
}

void ClusterWidget::onExecute()
{
    if (ui->lineEditExecute->text().trimmed().size() == 0)
    {
        return;
    }

    QList<QTreeWidgetItem*> selectedItems = ui->nodestreeWidget->selectedItems();
    foreach (QTreeWidgetItem *it, selectedItems)
    {
        int itr = it->text(5).toInt();
        ClusterNode node = cluster.nodes[itr];

        string cmdExecute = getSSHCmd(node.user, node.name, node.ssh_options);

        cmdExecute = cmdExecute + " "+ ui->lineEditExecute->text().toStdString();

        if (system(cmdExecute.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to run "+ ui->lineEditExecute->text().toStdString() + " on " + node.name;
            logError(QString(err.c_str()));
        }
    }
}

void ClusterWidget::onNodeSelectionChanged()
{
    if(ui->nodestreeWidget->selectedItems().isEmpty())
    {
        ui->runSelBtn->setDisabled(true);
        ui->stopSelBtn->setDisabled(true);
        ui->killSelBtn->setDisabled(true);
        ui->executeBtn->setDisabled(true);
    }
    else
    {
        ui->runSelBtn->setDisabled(false);
        ui->stopSelBtn->setDisabled(false);
        ui->killSelBtn->setDisabled(false);
        ui->executeBtn->setDisabled(false);
    }
}



void ClusterWidget::addRow(const std::string& name,const std::string& display,
                           const std::string& user, bool onOff, bool log, int id)
{
    QStringList stringList;
    stringList <<""<< QString(name.c_str()) << QString(display.c_str()) << QString(user.c_str())<< "" <<QString(std::to_string(id).c_str());
    QTreeWidgetItem* it = new QTreeWidgetItem(stringList);
    ui->nodestreeWidget->addTopLevelItem(it);
    ui->nodestreeWidget->setItemWidget((QTreeWidgetItem *) it, 4, new QCheckBox(this));

    //initialize checkboxes
    qobject_cast<QCheckBox*>(ui->nodestreeWidget->itemWidget((QTreeWidgetItem *)it, 4))->setChecked(log);

    //initialize icon
    if (onOff)
    {
        it->setIcon(0, QIcon(":/computer-available22.svg"));
    }
    else
    {
        it->setIcon(0, QIcon(":/computer-unavailable22.svg"));
    }

}

std::string ClusterWidget::getSSHCmd(const string &user, const string &host, const string &ssh_options)
{
    string cmd;
    cmd = "ssh -f";
    if (!ssh_options.empty())
    {
        cmd = cmd + " " + ssh_options;
    }
    if (user.empty())
    {
        cmd = cmd + " " + host;
    }
    else
    {
        cmd = cmd + " " + user + "@" +host;
    }

    return cmd;
}

bool ClusterWidget::checkNameserver()
{
    string name = ui->lineEditNs->text().toStdString();

    if (name.empty())
    {
        return false;
    }

    if (name.find("/") == std::string::npos)
    {
        name = "/" + name;
    }


    yarp::os::Bottle cmd, reply;
    cmd.addString("get");
    cmd.addString(name);
    cmd.addString("nameserver");
    bool ret = yarp::os::impl::NameClient::getNameClient().send(cmd, reply);
    if (!ret)
    {
        yError()<<"Manager::Cannot contact the NameClient";
        return false;
    }
    if (reply.size()==6)
    {
        if (reply.get(5).asBool())
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    else
    {
        return false;
    }
}

bool ClusterWidget::checkNode(const string &name)
{
    string portname = name;
    if (portname.find("/") == std::string::npos)
    {
        portname = "/" + portname;
    }

    yarp::os::Bottle cmd, reply;
    cmd.addString("get");
    cmd.addString(portname);
    cmd.addString("yarprun");
    bool ret = yarp::os::impl::NameClient::getNameClient().send(cmd, reply);
    if (!ret)
    {
        yError()<<"Manager::Cannot contact the NameClient";
        return false;
    }
    if (reply.size()==6)
    {
        if (reply.get(5).asBool())
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    else
    {
        return false;
    }

}

ClusterWidget::~ClusterWidget()
{
    if (clusLoader)
    {
        delete clusLoader;
        clusLoader = YARP_NULLPTR;
    }
    delete ui;
}
