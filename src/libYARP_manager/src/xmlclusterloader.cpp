/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/xmlclusterloader.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/ymm-dir.h>
#include <tinyxml.h>
#include <yarp/os/Value.h>
#ifdef WITH_GEOMETRY
#include <yarp/os/Property.h> // for parsing geometry information
#endif

#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <yarp/os/Network.h>
#include <yarp/manager/impl/textparser.h>



using namespace std;
using namespace yarp::manager;

/**
 * load only one application indicated by its xml file name
 */
XmlClusterLoader::XmlClusterLoader(std::string szFileName):confFile(szFileName)
{
}


XmlClusterLoader::~XmlClusterLoader()
{
}

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
        node != NULL; node = node->NextSiblingElement("node"))
    {
        ClusNode c_node;
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
        cluster.nodes.push_back(c_node);


    }
    _cluster = cluster;
    return true;

}
