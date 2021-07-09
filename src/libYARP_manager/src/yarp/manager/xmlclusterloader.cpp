/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/xmlclusterloader.h>
#include <yarp/manager/utility.h>
#include <dirent.h>
#include <tinyxml.h>
#include <yarp/os/Value.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <utility>
#include <yarp/os/Network.h>



using namespace std;
using namespace yarp::manager;

/**
 * load only one application indicated by its xml file name
 */
XmlClusterLoader::XmlClusterLoader(string szFileName) : confFile(std::move(szFileName))
{
}


XmlClusterLoader::~XmlClusterLoader() = default;

bool XmlClusterLoader::parseXmlFile(Cluster &_cluster)
{
    cluster.nodes.clear();
    ErrorLogger* logger  = ErrorLogger::Instance();
    TiXmlDocument doc(confFile);
    if (!doc.LoadFile())
    {
        OSTRINGSTREAM err;
        err<<"XmlClusterLoader: unable to load "<<confFile;
        logger->addError(err);
        return false;
    }

    /* retrieving root element */
    TiXmlElement *root = doc.RootElement();
    if (!root)
    {
        logger->addError("XmlClusterLoader: unable to find root element");
        return false;
    }

    if (root->ValueStr() != "cluster")
    {
        OSTRINGSTREAM err;
        err<<"XmlClusterLoader:No tag cluster found in"<<confFile;
        logger->addError(err);
        return false;
    }

    if (root->Attribute("name"))
    {
        cluster.name = root->Attribute("name");
    }

    if (root->Attribute("user"))
    {
        cluster.user = root->Attribute("user");
    }

    TiXmlElement *nameserver = root->FirstChildElement("nameserver");
    if (!nameserver)
    {
        OSTRINGSTREAM err;
        err<<"XmlClusterLoader:No tag nameserver found in"<<confFile;
        logger->addError(err);
        return false;
    }

    if (nameserver->Attribute("namespace"))
    {
        cluster.nameSpace = nameserver->Attribute("namespace");

    }

    if (nameserver->Attribute("node"))
    {
        cluster.nsNode = nameserver->Attribute("node");

    }

    if (nameserver->Attribute("ssh-options"))
    {
        cluster.ssh_options = nameserver->Attribute("ssh-options");

    }



    for (TiXmlElement* node = root->FirstChildElement("node");
        node != nullptr; node = node->NextSiblingElement("node"))
    {
        ClusterNode c_node;
        if (node->GetText())
        {
           c_node.name = node->GetText();
        }

        if (node->Attribute("display"))
        {
            c_node.display = true;
            c_node.displayValue = node->Attribute("display");
        }

        if (node->Attribute("user"))
        {
            c_node.user = node->Attribute("user");
        }
        else
        {
            c_node.user = cluster.user;
        }

        if (node->Attribute("ssh-options"))
        {
            c_node.ssh_options = node->Attribute("ssh-options");
        }

        if (node->Attribute("address"))
        {
            c_node.address = node->Attribute("address");
        }
        else
        {
            c_node.address = c_node.name;
        }
        cluster.nodes.push_back(c_node);


    }
    _cluster = cluster;
    return true;

}
