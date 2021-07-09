/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
#include <yarp/profiler/NetworkProfiler.h>

#include <algorithm>

#include <mainwindow.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::manager;

ClusterWidget::ClusterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterWidget), confFile(""), clusLoader(nullptr), checkNs(false)
{

#ifdef WIN32
    this->setDisabled(true);
    return;
#endif
    ui->setupUi(this);
    ui->executeBtn->setDisabled(true);
    ui->labelNs->setPixmap(QPixmap(":/close.svg").scaledToHeight(ui->checkRos->height()));

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
    connect(ui->lineEditExecute, SIGNAL(textChanged(QString)), SLOT(onExecuteTextChanged()));

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

    //check if yarpserver is running

    onCheckServer();

    QStringList l;
    //Adding nodes

    l.push_back(cluster.nsNode.c_str());
    int i{0};
    for (auto& node:cluster.nodes)
    {
        addRow(node.name, node.displayValue, node.user, node.address, node.onOff, node.log, i);
        i++;
        if (cluster.nsNode == node.name)
            continue;
        l.push_back(node.name.c_str());
    }

    // populate the execute combo box
    ui->executeComboBox->addItems(l);
    ui->executeComboBox->setEditable(true);

    ui->nsNodeComboBox->addItems(l);
    ui->nsNodeComboBox->setEditable(true);

    //check if all the nodes are up
    if (checkNs)
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
        int itr = it->text(6).toInt();
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
    checkNs = checkNameserver();
    if (checkNs) {
        ui->labelNs->setPixmap(QPixmap(":/apply.svg").scaledToHeight(ui->checkRos->height()));
    }
    else {
        ui->labelNs->setPixmap(QPixmap(":/close.svg").scaledToHeight(ui->checkRos->height()));
    }

    ui->checkRos->setDisabled(checkNs);
    ui->runServerBtn->setDisabled(checkNs);
    ui->nsNodeComboBox->setDisabled(checkNs);
    ui->stopServerBtn->setDisabled(!checkNs);
}

void ClusterWidget::onRunServer()
{
    updateServerEntries();

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
        std::string err = "ClusterWidget: failed to run the server on " + cluster.nsNode;
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
    updateServerEntries();

    auto count = std::count_if(cluster.nodes.begin(), cluster.nodes.end(),
                               [](const ClusterNode& e){ return e.onOff; });

    if (count > 0) {

        auto reply = QMessageBox::warning(this, "Shutting down yarpserver",
                                           "You have some yarprun on execution."
                                           " After shutting down yarpserver you might not be able to recover them."
                                           " Are you sure?",
                                           QMessageBox::Yes|QMessageBox::No);
        if (reply== QMessageBox::No) {
            return;
        }
    }

    string cmdStopServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);

    cmdStopServer = cmdStopServer + " killall yarpserver &";

    if (system(cmdStopServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to stop the server on " + cluster.nsNode;
        logError(QString(err.c_str()));
    }
    else
    {
        yarp::os::Time::delay(1.0);
        onCheckServer();
    }

    // if it fails to stop, kill it
    if (checkNs)
    {
        onKillServer();
    }
    else
    {
        std::string info = "ClusterWidget: yarpserver successfully stopped on "+ cluster.nsNode;
        logMessage(QString(info.c_str()));
    }
}

void ClusterWidget::onKillServer()
{
    updateServerEntries();

    string cmdKillServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);

    cmdKillServer = cmdKillServer + " killall -9 yarpserver &";

    if (system(cmdKillServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to kill the server on " + cluster.nsNode;
        logError(QString(err.c_str()));
    }
    else
    {
        std::string info = "ClusterWidget: yarpserver successfully killed on "+ cluster.nsNode;
        logMessage(QString(info.c_str()));
    }


}

void ClusterWidget::onRunSelected()
{
    QList<QTreeWidgetItem*> selectedItems = ui->nodestreeWidget->selectedItems();
    foreach (QTreeWidgetItem *it, selectedItems)
    {
        int itr = it->text(6).toInt();
        ClusterNode node = cluster.nodes[itr];
        string portName = node.name;

        if (portName.find('/') == std::string::npos)
        {
            portName.insert(0, 1, '/');
        }

        if (node.onOff)
        {
            continue;
        }

        string cmdRunYarprun = getSSHCmd(node.user, node.address, node.ssh_options);
        if (node.display)
        {
            cmdRunYarprun.append(" 'export DISPLAY=").append(node.displayValue).append(" && ");

        }
        if (qobject_cast<QCheckBox*>(ui->nodestreeWidget->itemWidget((QTreeWidgetItem *)it, 5))->isChecked())
        {
            cmdRunYarprun.append(" yarprun --server ").append(portName).append(" --log 2>&1 2>/tmp/yarprunserver.log");
        }
        else
        {
            cmdRunYarprun.append(" yarprun --server ").append(portName).append(" 2>&1 2>/tmp/yarprunserver.log");
        }

        if (node.display)
        {
            cmdRunYarprun.append("'");
        }
        if (system(cmdRunYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to run yarprun on " + node.name;
            logError(QString(err.c_str()));
        }
        else
        {
            std::string info = "ClusterWidget: yarprun successfully executed on "+ node.name;
            logMessage(QString(info.c_str()));
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
        int itr = it->text(6).toInt();
        ClusterNode node = cluster.nodes[itr];
        if (!node.onOff)
        {
            continue;
        }
        string portName = node.name;
        if (portName.find('/') == std::string::npos)
        {
            portName.insert(0, 1, '/');
        }

        string cmdStopYarprun = getSSHCmd(node.user, node.address, node.ssh_options);

        cmdStopYarprun.append(" yarprun --exit --on ").append(portName).append(" &");

        if (system(cmdStopYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to stop yarprun on " + node.name;
            logError(QString(err.c_str()));
        }
        else
        {
            std::string info = "ClusterWidget: yarprun successfully stopped on "+ node.name;
            logMessage(QString(info.c_str()));
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
        int itr = it->text(6).toInt();
        ClusterNode node = cluster.nodes[itr];
        if (!node.onOff)
        {
            continue;
        }

        string cmdKillYarprun = getSSHCmd(node.user, node.address, node.ssh_options);

        cmdKillYarprun.append(" killall -9 yarprun &");

        if (system(cmdKillYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to kill yarprun on " + node.name;
            logError(QString(err.c_str()));
        }
        else
        {
            std::string info = "ClusterWidget: yarprun successfully killed on "+ node.name;
            logMessage(QString(info.c_str()));
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

    auto nodeName = ui->executeComboBox->currentText();

    if (nodeName.trimmed().size() == 0)
    {
        return;
    }

    auto nodeItr = std::find_if(cluster.nodes.begin(), cluster.nodes.end(),
                               [&nodeName](const ClusterNode& n){ return n.name == nodeName.toStdString(); });


    if (nodeItr == cluster.nodes.end())
    {
        return;
    }

    auto node = *nodeItr;
    auto command = ui->lineEditExecute->text().toStdString();

    string cmdExecute = getSSHCmd(node.user, node.address, node.ssh_options);

    cmdExecute.append(" ").append(command);

    if (system(cmdExecute.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to run "+ command + " on " + node.name;
        logError(QString(err.c_str()));
    }
    else
    {
        std::string info = "ClusterWidget: command "+ command + " successfully executed on " + node.name;
        logMessage(QString(info.c_str()));
    }

    ui->lineEditExecute->clear();
}

void ClusterWidget::onNodeSelectionChanged()
{
    if(ui->nodestreeWidget->selectedItems().isEmpty())
    {
        ui->runSelBtn->setDisabled(true);
        ui->stopSelBtn->setDisabled(true);
        ui->killSelBtn->setDisabled(true);
    }
    else
    {
        ui->runSelBtn->setDisabled(!checkNs);
        ui->stopSelBtn->setDisabled(!checkNs);
        ui->killSelBtn->setDisabled(!checkNs);
    }
}


void ClusterWidget::onExecuteTextChanged()
{
    if (ui->lineEditExecute->text().trimmed().size() > 0)
        ui->executeBtn->setDisabled(false);
    else
        ui->executeBtn->setDisabled(true);
}



void ClusterWidget::addRow(const std::string& name,const std::string& display,
                           const std::string& user, const std::string& address,
                           bool onOff, bool log, int id)
{
    QStringList stringList;
    stringList <<""<< QString(name.c_str()) << QString(display.c_str()) << QString(user.c_str()) << QString(address.c_str())<< "" <<QString(std::to_string(id).c_str());
    auto* it = new QTreeWidgetItem(stringList);
    ui->nodestreeWidget->addTopLevelItem(it);
    ui->nodestreeWidget->setItemWidget((QTreeWidgetItem *) it, 5, new QCheckBox(this));

    //initialize checkboxes
    qobject_cast<QCheckBox*>(ui->nodestreeWidget->itemWidget((QTreeWidgetItem *)it, 5))->setChecked(log);

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

    if (name.find('/') == std::string::npos)
    {
        name = "/" + name;
    }


    if (!NetworkBase::checkNetwork(2.0))
    {
        yError()<<"ClusterWidget: yarpserver is not running";
        return false;
    }


    yarp::os::Bottle cmd, reply;
    cmd.addString("get");
    cmd.addString(name);
    cmd.addString("nameserver");
    bool ret = yarp::os::impl::NameClient::getNameClient().send(cmd, reply);
    if (!ret)
    {
        yError()<<"ClusterWidget: Cannot contact the NameClient";
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
    if (portname.find('/') == std::string::npos)
    {
        portname = "/" + portname;
    }

    if (!NetworkBase::checkNetwork(2.0))
    {
        yError()<<"ClusterWidget: yarpserver is not running";
        return false;
    }

    yarp::profiler::NetworkProfiler::PortDetails dummy;
    if (! yarp::profiler::NetworkProfiler::getPortDetails(portname, dummy))
    {
        yError()<<"ClusterWidget: port"<<portname<<"is not responding";
        return false;
    }


    yarp::os::Bottle cmd, reply;
    cmd.addString("get");
    cmd.addString(portname);
    cmd.addString("yarprun");
    bool ret = yarp::os::impl::NameClient::getNameClient().send(cmd, reply);
    if (!ret)
    {
        yError()<<"ClusterWidget: Cannot contact the NameClient";
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

void ClusterWidget::updateServerEntries()
{
    // remove all the whitespaces
    cluster.user   = ui->lineEditUser->text().simplified().trimmed().toStdString();
    cluster.nsNode = ui->nsNodeComboBox->currentText().simplified().trimmed().toStdString();
}

ClusterWidget::~ClusterWidget()
{
    if (clusLoader)
    {
        delete clusLoader;
        clusLoader = nullptr;
    }
    delete ui;
}
