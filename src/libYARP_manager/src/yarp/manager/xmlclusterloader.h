/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_XMLCLUSTERLOADER_H
#define YARP_MANAGER_XMLCLUSTERLOADER_H

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/manifestloader.h>



namespace yarp {
namespace manager {

struct ClusterNode
{
    std::string name = "";
    std::string address = "";
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
    std::vector<ClusterNode> nodes;
};

/**
 * Class XmlClusterLoader
 */
class XmlClusterLoader {

public:
    XmlClusterLoader(std::string szFileName);
    virtual ~XmlClusterLoader();
    bool parseXmlFile(Cluster& _cluster);

protected:

private:
    std::string confFile;
    Cluster     cluster;
};

} // namespace manager
} // namespace yarp


#endif // YARP_MANAGER_XMLCLUSTERLOADER_H
