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

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>

using namespace std;
using namespace yarp::os;

ClusterWidget::ClusterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClusterWidget)
{

#ifdef WIN32
    this->setDisabled(true);
    return;
#endif
    confFile = "";
    ui->setupUi(this);

    ui->gridLayout->addWidget(new QLabel("Name"), 0, 0);
    ui->gridLayout->addWidget(new QLabel("Display"), 0, 1);
    ui->gridLayout->addWidget(new QLabel("User"), 0, 2);
    ui->gridLayout->addWidget(new QLabel("On/off"), 0, 3);
    ui->gridLayout->addWidget(new QLabel("Log"), 0, 4);
    ui->gridLayout->addWidget(new QLabel("Select"), 0, 5);

    //checking for the cluster-config.xml

    ResourceFinder rf;
    rf.setDefaultContext("iCubCluster");

    confFile = rf.findFileByName("cluster-config.xml");

    if(confFile.empty())
    {
        yError()<<"Unable to find cluster-config.xml in context iCubCluster";
        this->setDisabled(true);
        return;
    }

    //Parsing config file
    if(!parseConfigFile())
    {
        yError()<<"Unable parse cluster-config.xml in context iCubCluster";
        this->setDisabled(true);
        return;
    }

    ui->lineEditUser->setText(cluster.user.c_str());
    ui->lineEditNs->setText(cluster.nameSpace.c_str());
    ui->lineEditNsNode->setText(cluster.nsNode.c_str());

    //check if yarpserver is running

    //check if yarprun is running

    //Adding nodes

    for(size_t i = 0; i<cluster.nodes.size(); i++)
    {
        ClusNode node = cluster.nodes[i];
        addRow(node.name, node.displayValue, node.user, node.onOff, node.log);
    }

}

void ClusterWidget::addRow(string name, string display, string user,
                           bool onOff, bool log, bool select)
{
    int rowCount = ui->gridLayout->rowCount();
    ui->gridLayout->addWidget(new QLineEdit(name.c_str()), rowCount, 0);
    ui->gridLayout->addWidget(new QLineEdit(display.c_str()), rowCount, 1);
    ui->gridLayout->addWidget(new QLineEdit(user.c_str()), rowCount, 2);
    ui->gridLayout->addWidget(new QCheckBox(), rowCount, 3);
    ui->gridLayout->addWidget(new QCheckBox(), rowCount, 4);
    ui->gridLayout->addWidget(new QCheckBox(), rowCount, 5);

    //initialize checkboxes
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,3)->widget())->setChecked(onOff);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,4)->widget())->setChecked(log);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,5)->widget())->setChecked(select);

    //read only
    qobject_cast<QLineEdit*>(ui->gridLayout->itemAtPosition(rowCount,0)->widget())->setReadOnly(true);
    qobject_cast<QLineEdit*>(ui->gridLayout->itemAtPosition(rowCount,1)->widget())->setReadOnly(true);
    qobject_cast<QLineEdit*>(ui->gridLayout->itemAtPosition(rowCount,2)->widget())->setReadOnly(true);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,3)->widget())->setAttribute(Qt::WA_TransparentForMouseEvents);
    qobject_cast<QCheckBox*>(ui->gridLayout->itemAtPosition(rowCount,3)->widget())->setFocusPolicy(Qt::NoFocus);

}

std::string ClusterWidget::getSSHCmd(std::string user, std::string host, std::string ssh_options)
{
    return "";
}

bool ClusterWidget::parseConfigFile()
{
    TiXmlDocument doc(confFile);
    if(!doc.LoadFile())
    {
        yError()<<"XmlParser: unable to load"<<confFile;
        return false;
    }

    /* retrieving root element */
    TiXmlElement *root = doc.RootElement();
    if(!root)
    {
        yError()<<"XmlParser: unable to find root element";
        return false;
    }

    if(root->ValueStr() != "cluster")
    {
        yError()<<"No tag cluster found in"<<confFile;
        return false;
    }

    if(root->Attribute("name"))
    {
        cluster.name = root->Attribute("name");
    }

    if(root->Attribute("user"))
    {
        cluster.user = root->Attribute("user");
    }

    TiXmlElement *nameserver = root->FirstChildElement("nameserver");
    if(!nameserver)
    {
        yError()<<"No tag nameserver found in"<<confFile;
        return false;
    }

    if(nameserver->Attribute("namespace"))
    {
        cluster.nameSpace = nameserver->Attribute("namespace");

    }

    if(nameserver->Attribute("node"))
    {
        cluster.nsNode = nameserver->Attribute("node");

    }

    if(nameserver->Attribute("ssh-options"))
    {
        cluster.ssh_options = nameserver->Attribute("ssh-options");

    }



    for(TiXmlElement* node = root->FirstChildElement("node");
        node != NULL; node = node->NextSiblingElement("node"))
    {
        ClusNode c_node;
        if(node->GetText())
        {
           c_node.name = node->GetText();
        }

        if(node->Attribute("display"))
        {
            c_node.display = true;
            c_node.displayValue = node->Attribute("display");
        }

        if(node->Attribute("user"))
            c_node.user = node->Attribute("user");
        else
            c_node.user = cluster.user;

        if(node->Attribute("ssh-options"))
            c_node.ssh_options = node->Attribute("ssh-options");
        cluster.nodes.push_back(c_node);

    }



    return true;

}

ClusterWidget::~ClusterWidget()
{
    delete ui;
}
