/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "clusterWidget.h"
#include "ui_clusterWidget.h"
#include "iostream"
#include <QLineEdit>
#include <QCheckBox>
#include <QBrush>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QStringList>

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/NameClient.h>
#include <yarp/profiler/NetworkProfiler.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cctype>
#include <set>
#include <sstream>

#include <mainwindow.h>

using namespace yarp::os;
using namespace yarp::manager;

namespace {

std::string shellQuote(const std::string& value)
{
    std::string quoted = "'";
    for (char c : value)
    {
        if (c == '\'')
        {
            quoted += "'\\''";
        }
        else
        {
            quoted += c;
        }
    }
    quoted += "'";
    return quoted;
}

std::vector<std::string> splitTabs(const std::string& line)
{
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, '\t'))
    {
        fields.push_back(field);
    }
    return fields;
}

std::string sanitizedFilePart(std::string value)
{
    for (char& c : value)
    {
        if (!std::isalnum(static_cast<unsigned char>(c)))
        {
            c = '_';
        }
    }
    return value;
}

std::string yarprunLogFile(const std::string& portName)
{
    return "/tmp/yarpmanager_yarprun_" + sanitizedFilePart(portName) + ".log";
}

std::string runtimeName(const yarp::manager::ClusterNode& node)
{
    if (!node.docker.empty()) {
        return "docker";
    }
    if (!node.conda.empty()) {
        return "conda";
    }
    if (!node.pixi.empty()) {
        return "pixi";
    }
    return "";
}

std::string runtimeTarget(const yarp::manager::ClusterNode& node)
{
    if (!node.docker.empty()) {
        return node.docker;
    }
    if (!node.conda.empty()) {
        return node.conda;
    }
    if (!node.pixi.empty()) {
        return node.pixi;
    }
    return "";
}

int runtimeCount(const yarp::manager::ClusterNode& node)
{
    return (!node.docker.empty() ? 1 : 0) +
           (!node.conda.empty() ? 1 : 0) +
           (!node.pixi.empty() ? 1 : 0);
}

} // namespace

ClusterWidget::ClusterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterWidget),
    confFile(""),
    clusLoader(nullptr),
    checkNs(false),
    dockerGroupBox(nullptr),
    dockerTreeWidget(nullptr),
    dockerRefreshBtn(nullptr),
    dockerStartBtn(nullptr),
    dockerStopBtn(nullptr)
{

#ifdef WIN32
    this->setDisabled(true);
    return;
#endif
    ui->setupUi(this);
    ui->nodestreeWidget->setColumnCount(8);
    ui->nodestreeWidget->setHeaderLabels(QStringList() << "Status" << "Name" << "Display" << "User" << "Address" << "Runtime" << "Target" << "Log");
    ui->executeBtn->setDisabled(true);
    ui->labelNs->setPixmap(QPixmap(":/close.svg").scaledToHeight(ui->checkRos->height()));
    addDockerControls();

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
    connect(ui->nodestreeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem*, int column) {
        if (column == 5) {
            updateYarprunDockerColors();
        }
    });
    connect(ui->lineEditExecute, SIGNAL(textChanged(QString)), SLOT(onExecuteTextChanged()));
    connect(dockerTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateDockerButtons()));

}

void ClusterWidget::setConfigFile(const std::string &_confFile)
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
        addRow(node.name, node.displayValue, node.user, node.address, runtimeName(node), runtimeTarget(node), node.onOff, node.log, i);
        i++;
        if (cluster.nsNode == node.name) {
            continue;
        }
        l.push_back(node.name.c_str());
    }

    // populate the execute combo box
    ui->executeComboBox->addItems(l);
    ui->executeComboBox->setEditable(true);

    ui->nsNodeComboBox->addItems(l);
    ui->nsNodeComboBox->setEditable(true);

    onRefreshDockerContainers();

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
        int itr = getNodeIndex(it);
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
    updateYarprunDockerColors();

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

    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    std::string cmdRunServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);
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
        logger->addError(err);
        reportErrors();
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

    std::string cmdStopServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);

    cmdStopServer = cmdStopServer + " killall yarpserver &";

    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

    if (system(cmdStopServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to stop the server on " + cluster.nsNode;
        logger->addError(err);
        reportErrors();
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

    std::string cmdKillServer = getSSHCmd(cluster.user, cluster.nsNode, cluster.ssh_options);

    cmdKillServer = cmdKillServer + " killall -9 yarpserver &";

    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    if (system(cmdKillServer.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to kill the server on " + cluster.nsNode;
        logger->addError(err);
        reportErrors();
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
        int itr = getNodeIndex(it);
        ClusterNode node = cluster.nodes[itr];
        node.docker.clear();
        node.conda.clear();
        node.pixi.clear();

        std::string runtime = it->text(5).trimmed().toStdString();
        std::string target = it->text(6).trimmed().toStdString();

        if (runtime == "docker") {
            node.docker = target;
        } else if (runtime == "conda") {
            node.conda = target;
        } else if (runtime == "pixi") {
            node.pixi = target;
        }
        std::string portName = node.name;

        if (portName.find('/') == std::string::npos)
        {
            portName.insert(0, 1, '/');
        }

        if (node.onOff)
        {
            continue;
        }

        yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();

        if (runtimeCount(node) > 1)
        {
            logger->addError("ClusterWidget: only one runtime can be specified among docker, conda and pixi.");
            reportErrors();
            continue;
        }

        if (!node.docker.empty() && !isDockerRunning(node, node.docker))
        {
            logger->addError("ClusterWidget: cannot run yarprun inside docker " + node.docker +
                            " because the container is not running. Please start the docker container first.");
            reportErrors();
            continue;
        }

        if (!node.pixi.empty() && !checkPixiRuntime(node))
        {
            reportErrors();
            continue;
        }

        std::string cmdRunYarprun = getSSHCmd(node.user, node.address, node.ssh_options);
        bool log = qobject_cast<QCheckBox*>(ui->nodestreeWidget->itemWidget((QTreeWidgetItem *)it, 7))->isChecked();
        std::string logFile = yarprunLogFile(portName);
        std::string cleanupOutput;
        runRemoteCommand(node, "rm -f " + shellQuote(logFile), cleanupOutput);

        cmdRunYarprun.append(" ").append(shellQuote(buildYarprunCommand(node, portName, log, logFile)));
        if (system(cmdRunYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to run yarprun on " + node.name;
            logger->addError(err);
            reportErrors();
        }
        else
        {
            yarp::os::Time::delay(1.0);
            if (!checkNode(node.name))
            {
                std::string tailOutput;
                runRemoteCommand(node, "test -f " + shellQuote(logFile) + " && tail -n 80 " + shellQuote(logFile), tailOutput);
                logger->addError("ClusterWidget: yarprun did not start on " + node.name + ". Remote log " + logFile + ":\n" + tailOutput);
                reportErrors();
                continue;
            }

            std::string info = "ClusterWidget: yarprun successfully executed on " + node.name + ". Remote log: " + logFile;
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
        int itr = getNodeIndex(it);
        ClusterNode node = cluster.nodes[itr];
        if (!node.onOff)
        {
            continue;
        }
        std::string portName = node.name;
        if (portName.find('/') == std::string::npos)
        {
            portName.insert(0, 1, '/');
        }

        std::string cmdStopYarprun = getSSHCmd(node.user, node.address, node.ssh_options);

        cmdStopYarprun.append(" yarprun --exit --on ").append(portName).append(" &");

        yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
        if (system(cmdStopYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to stop yarprun on " + node.name;
            logger->addError(err);
            reportErrors();
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
        int itr = getNodeIndex(it);
        ClusterNode node = cluster.nodes[itr];
        if (!node.onOff)
        {
            continue;
        }

        std::string cmdKillYarprun = getSSHCmd(node.user, node.address, node.ssh_options);

        cmdKillYarprun.append(" killall -9 yarprun &");

        yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
        if (system(cmdKillYarprun.c_str()) != 0)
        {
            std::string err = "ClusterWidget: failed to kill yarprun on " + node.name;
            logger->addError(err);
            reportErrors();
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

    std::string cmdExecute = getSSHCmd(node.user, node.address, node.ssh_options);

    cmdExecute.append(" ").append(command);

    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    if (system(cmdExecute.c_str()) != 0)
    {
        std::string err = "ClusterWidget: failed to run "+ command + " on " + node.name;
        logger->addError(err);
        reportErrors();
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
    if (ui->lineEditExecute->text().trimmed().size() > 0) {
        ui->executeBtn->setDisabled(false);
    } else {
        ui->executeBtn->setDisabled(true);
    }
}



void ClusterWidget::addRow(const std::string& name,const std::string& display,
                           const std::string& user, const std::string& address,
                           const std::string& runtime, const std::string& target, bool onOff, bool log, int id)
{
    QStringList stringList;
    stringList << "" << QString(name.c_str()) << QString(display.c_str()) << QString(user.c_str()) << QString(address.c_str()) << QString(runtime.c_str()) << QString(target.c_str()) << "";
    auto* it = new QTreeWidgetItem(stringList);
    it->setData(0, Qt::UserRole, id);
    it->setFlags(it->flags() | Qt::ItemIsEditable);
    ui->nodestreeWidget->addTopLevelItem(it);
    ui->nodestreeWidget->setItemWidget((QTreeWidgetItem *) it, 7, new QCheckBox(this));

    //initialize checkboxes
    qobject_cast<QCheckBox*>(ui->nodestreeWidget->itemWidget((QTreeWidgetItem *)it, 7))->setChecked(log);

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

int ClusterWidget::getNodeIndex(QTreeWidgetItem* item) const
{
    return item->data(0, Qt::UserRole).toInt();
}

std::string ClusterWidget::buildYarprunCommand(const yarp::manager::ClusterNode& node, const std::string& portName, bool log, const std::string& logFile)
{
    std::string yarprunCommand;
    if (node.display)
    {
        yarprunCommand.append("export DISPLAY=").append(node.displayValue).append(" && ");
    }

    yarprunCommand.append("yarprun --server ").append(portName);
    if (log)
    {
        yarprunCommand.append(" --log");
    }

    if (!node.docker.empty())
    {
        return "docker exec " + shellQuote(node.docker) +
               " sh -lc " + shellQuote(yarprunCommand) +
               " > " + shellQuote(logFile) + " 2>&1";
    }

    if (!node.conda.empty())
    {
        return "bash -lc " + shellQuote(
            "for conda_sh in "
            "\"$HOME/miniforge3/etc/profile.d/conda.sh\" "
            "\"$HOME/miniconda3/etc/profile.d/conda.sh\" "
            "\"$HOME/anaconda3/etc/profile.d/conda.sh\" "
            "\"/opt/conda/etc/profile.d/conda.sh\"; do "
            "[ -f \"$conda_sh\" ] && source \"$conda_sh\" && break; "
            "done; "
            "if ! command -v conda >/dev/null 2>&1; then "
            "echo \"conda command not found. Please install conda or add conda.sh to one of the standard locations.\" >&2; "
            "exit 1; "
            "fi; "
            "conda activate " + shellQuote(node.conda) + " && " +
            yarprunCommand) +
            " > " + shellQuote(logFile) + " 2>&1";
    }

    if (!node.pixi.empty())
    {
        return "bash -lc " + shellQuote(
            "export PATH=\"$HOME/.pixi/bin:$HOME/.cargo/bin:$PATH\" && "
            "cd " + shellQuote(node.pixi) + " && "
            "pixi run " + yarprunCommand) +
            " > " + shellQuote(logFile) + " 2>&1";
    }

    return yarprunCommand + " > " + shellQuote(logFile) + " 2>&1";
}

bool ClusterWidget::runRemoteCommand(const yarp::manager::ClusterNode& node, const std::string& command, std::string& output)
{
    output.clear();
    std::string cmd = getSSHCmd(node.user, node.address, node.ssh_options, false);
    cmd += " ";
    cmd += shellQuote(command);
    cmd += " 2>&1";

    std::array<char, 512> buffer{};
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        output = "unable to execute ssh command";
        return false;
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        output += buffer.data();
    }

    return pclose(pipe) == 0;
}

bool ClusterWidget::checkPixiRuntime(const yarp::manager::ClusterNode& node)
{
    std::string output;
    std::string script =
        "bash -lc " + shellQuote(
            "export PATH=\"$HOME/.pixi/bin:$HOME/.cargo/bin:$PATH\"; "
            "if [ ! -d " + shellQuote(node.pixi) + " ]; then "
            "echo \"pixi project directory not found: " + node.pixi + "\"; exit 1; "
            "fi; "
            "if [ ! -f " + shellQuote(node.pixi + "/pixi.toml") + " ]; then "
            "echo \"pixi.toml not found in: " + node.pixi + "\"; exit 1; "
            "fi; "
            "if ! command -v pixi >/dev/null 2>&1; then "
            "echo \"pixi command not found. Install pixi or add it to PATH on host " + node.address + ".\"; exit 1; "
            "fi; "
            "cd " + shellQuote(node.pixi) + " && pixi info >/dev/null");

    if (!runRemoteCommand(node, script, output))
    {
        yarp::manager::ErrorLogger::Instance()->addError("Pixi: cannot run yarprun on " + node.name + ": " + output);
        return false;
    }

    return true;
}

bool ClusterWidget::isDockerRunning(const yarp::manager::ClusterNode& node, const std::string& dockerName)
{
    if (dockerName.empty())
    {
        return false;
    }

    std::string cmd = getSSHCmd(node.user, node.address, node.ssh_options, false);
    cmd += " ";
    cmd += shellQuote("docker ps --format '{{.Names}}'");
    cmd += " 2>&1";

    std::array<char, 512> buffer{};
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        yarp::manager::ErrorLogger::Instance()->addError("Docker: unable to execute docker ps on " + node.name);
        return false;
    }

    std::string output;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        output += buffer.data();
    }

    int ret = pclose(pipe);
    if (ret != 0)
    {
        yarp::manager::ErrorLogger::Instance()->addError("Docker: docker ps failed on " + node.name + ": " + output);
        return false;
    }

    std::stringstream lines(output);
    std::string line;
    while (std::getline(lines, line))
    {
        if (line == dockerName)
        {
            return true;
        }
    }

    return false;
}

void ClusterWidget::updateYarprunDockerColors()
{
    for (int i = 0; i < ui->nodestreeWidget->topLevelItemCount(); i++)
    {
        auto* item = ui->nodestreeWidget->topLevelItem(i);
        int nodeIndex = getNodeIndex(item);
        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= cluster.nodes.size())
        {
            continue;
        }

        std::string runtime = item->text(5).trimmed().toStdString();
        std::string target = item->text(6).trimmed().toStdString();

        bool green = false;
        if (runtime == "docker" && !target.empty())
        {
            green = isDockerRunning(cluster.nodes[nodeIndex], target);
        }

        item->setForeground(6, green ? QBrush(Qt::darkGreen) : QBrush(Qt::black));
    }
}

void ClusterWidget::addDockerControls()
{
    dockerGroupBox = new QGroupBox("Docker containers", this);
    auto* dockerLayout = new QVBoxLayout(dockerGroupBox);
    auto* buttonsLayout = new QHBoxLayout();

    dockerRefreshBtn = new QPushButton("Refresh", dockerGroupBox);
    dockerStartBtn = new QPushButton("Start", dockerGroupBox);
    dockerStopBtn = new QPushButton("Stop", dockerGroupBox);
    dockerStartBtn->setDisabled(true);
    dockerStopBtn->setDisabled(true);

    buttonsLayout->addWidget(dockerRefreshBtn);
    buttonsLayout->addWidget(dockerStartBtn);
    buttonsLayout->addWidget(dockerStopBtn);

    dockerTreeWidget = new QTreeWidget(dockerGroupBox);
    dockerTreeWidget->setColumnCount(7);
    dockerTreeWidget->setHeaderLabels(QStringList() << "Host" << "Container" << "Status" << "ID" << "Address" << "User" << "SSH options");
    dockerTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    dockerTreeWidget->setRootIsDecorated(false);
    dockerTreeWidget->setAlternatingRowColors(true);
    dockerTreeWidget->setColumnHidden(4, true);
    dockerTreeWidget->setColumnHidden(5, true);
    dockerTreeWidget->setColumnHidden(6, true);
    dockerTreeWidget->header()->setStretchLastSection(false);
    dockerTreeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    dockerTreeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    dockerTreeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    dockerTreeWidget->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    dockerLayout->addLayout(buttonsLayout);
    dockerLayout->addWidget(dockerTreeWidget);
    ui->verticalLayout_5->addWidget(dockerGroupBox);

    connect(dockerRefreshBtn, SIGNAL(clicked(bool)), this, SLOT(onRefreshDockerContainers()));
    connect(dockerStartBtn, SIGNAL(clicked(bool)), this, SLOT(onStartSelectedDockerContainers()));
    connect(dockerStopBtn, SIGNAL(clicked(bool)), this, SLOT(onStopSelectedDockerContainers()));
}

void ClusterWidget::updateDockerButtons()
{
    const bool hasSelection = dockerTreeWidget && !dockerTreeWidget->selectedItems().isEmpty();
    dockerStartBtn->setDisabled(!hasSelection);
    dockerStopBtn->setDisabled(!hasSelection);
}

std::vector<yarp::manager::DockerContainer> ClusterWidget::getDockerContainers(const yarp::manager::ClusterNode& node)
{
    std::vector<yarp::manager::DockerContainer> containers;
    std::string cmd = getSSHCmd(node.user, node.address, node.ssh_options, false);
    cmd += " ";
    cmd += shellQuote("docker ps -a --format '{{.ID}}\t{{.Names}}\t{{.Status}}'");
    cmd += " 2>&1";

    std::array<char, 512> buffer{};
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
        yarp::manager::ErrorLogger::Instance()->addError("Docker: unable to execute docker ps on " + node.name);
        return containers;
    }

    std::string output;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        output += buffer.data();
    }

    int ret = pclose(pipe);
    if (ret != 0)
    {
        yarp::manager::ErrorLogger::Instance()->addError("Docker: docker ps failed on " + node.name + ": " + output);
        return containers;
    }

    std::stringstream lines(output);
    std::string line;
    while (std::getline(lines, line))
    {
        if (line.empty())
        {
            continue;
        }

        auto fields = splitTabs(line);
        if (fields.size() < 3)
        {
            continue;
        }

        yarp::manager::DockerContainer container;
        container.id = fields[0];
        container.name = fields[1];
        container.status = fields[2];
        container.host = node.address;
        container.user = node.user;
        container.ssh_options = node.ssh_options;
        containers.push_back(container);
    }

    return containers;
}

bool ClusterWidget::runDockerCommand(const yarp::manager::DockerContainer& container, const std::string& command)
{
    std::string cmd = getSSHCmd(container.user, container.host, container.ssh_options);
    cmd += " ";
    cmd += shellQuote(command + " " + shellQuote(container.name));
    return system(cmd.c_str()) == 0;
}

void ClusterWidget::onRefreshDockerContainers()
{
    if (!dockerTreeWidget)
    {
        return;
    }

    dockerTreeWidget->clear();
    std::set<std::string> visitedHosts;
    for (const auto& node : cluster.nodes)
    {
        std::string key = node.user + "@" + node.address + " " + node.ssh_options;
        if (!visitedHosts.insert(key).second)
        {
            continue;
        }

        auto containers = getDockerContainers(node);
        for (const auto& container : containers)
        {
            QStringList row;
            row << QString(container.host.c_str())
                << QString(container.name.c_str())
                << QString(container.status.c_str())
                << QString(container.id.c_str())
                << QString(container.host.c_str())
                << QString(container.user.c_str())
                << QString(container.ssh_options.c_str());
            dockerTreeWidget->addTopLevelItem(new QTreeWidgetItem(row));
        }
    }

    dockerTreeWidget->resizeColumnToContents(0);
    dockerTreeWidget->resizeColumnToContents(1);
    dockerTreeWidget->resizeColumnToContents(3);
    updateDockerButtons();
    updateYarprunDockerColors();
    reportErrors();
}

void ClusterWidget::onStartSelectedDockerContainers()
{
    for (auto* item : dockerTreeWidget->selectedItems())
    {
        yarp::manager::DockerContainer container;
        container.name = item->text(1).toStdString();
        container.host = item->text(4).toStdString();
        container.user = item->text(5).toStdString();
        container.ssh_options = item->text(6).toStdString();

        if (!runDockerCommand(container, "docker start"))
        {
            yarp::manager::ErrorLogger::Instance()->addError("Docker: failed to start " + container.name + " on " + container.host);
        }
    }
    reportErrors();
    onRefreshDockerContainers();
}

void ClusterWidget::onStopSelectedDockerContainers()
{
    for (auto* item : dockerTreeWidget->selectedItems())
    {
        yarp::manager::DockerContainer container;
        container.name = item->text(1).toStdString();
        container.host = item->text(4).toStdString();
        container.user = item->text(5).toStdString();
        container.ssh_options = item->text(6).toStdString();

        if (!runDockerCommand(container, "docker stop"))
        {
            yarp::manager::ErrorLogger::Instance()->addError("Docker: failed to stop " + container.name + " on " + container.host);
        }
    }
    reportErrors();
    onRefreshDockerContainers();
}

std::string ClusterWidget::getSSHCmd(const std::string &user, const std::string &host, const std::string &ssh_options, bool background)
{
    std::string cmd;
    cmd = background ? "ssh -f" : "ssh";
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
    std::string name = ui->lineEditNs->text().toStdString();

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

bool ClusterWidget::checkNode(const std::string &name)
{
    std::string portname = name;
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

void ClusterWidget::reportErrors()
{
    yarp::manager::ErrorLogger* logger  = yarp::manager::ErrorLogger::Instance();
    if (logger->errorCount() || logger->warningCount())
    {
        const char* err;
        while((err=logger->getLastError()))
        {
            QString msg = QString("ClusterWidget: %1").arg(err);
            emit logError(msg);
        }
    }
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
