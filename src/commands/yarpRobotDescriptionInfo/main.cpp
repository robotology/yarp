/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRobotDescription.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include<fstream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

namespace
{
    YARP_LOG_COMPONENT(YARPDESCINFO, "yarp.yarpRobotDescriptionInfo")
}

void display_help ()
{
    yCInfo(YARPDESCINFO) << "This is the tool yarpRobotDescriptionInfo.";
    yCInfo(YARPDESCINFO) << "By default it prints on screen all the connections found on the yarp network.";
    yCInfo(YARPDESCINFO) << "use the --file <filename> to choose the name of the output file. (default: outputfile.dot) ";
    yCInfo(YARPDESCINFO) << "use the --remote <remote_port> to select a specific the robotDescription_nws_yarp rpc port (default: /robotDescription_nws_yarp/rpc)";
    yCInfo(YARPDESCINFO) << "use an online dot editor (e.g. https://dreampuf.github.io/GraphvizOnline) to visualize the graph.";
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    yarp::os::Property p;
    if (argc > 1) {
        p.fromCommand(argc, argv);
    }

    std::string filename = "outputfile.dot";
    std::string remotename = "/robotDescription_nws_yarp/rpc";

    if (p.check("file"))
    {
        filename = p.find("file").asString();
    }
    if (p.check("remote")) {
        remotename = p.find("remote").asString();
    }

    yarp::dev::PolyDriver pp;
    Property pcfg;
    pcfg.put("device", "robotDescription_nwc_yarp");
    pcfg.put("local",  "/robotDescription_nwc_yarp/rpc");
    pcfg.put("remote", remotename);
    pp.open(pcfg);
    IRobotDescription* idesc = nullptr;
    pp.view(idesc);

    std::vector<yarp::dev::DeviceDescription> devlist;
    yarp::dev::ReturnValue ret;
    if (idesc)
    {
        ret = idesc->getAllDevices(devlist);
    }

    std::ofstream file;
    file.open(filename.c_str(), std::ios::out);
    file << "digraph G" << endl;
    file << "{" << endl;
    file << "    splines = \"compound\"" << endl;
    file << "        rankdir= \"TB\"" << endl;
    file << "        subgraph cluster_net" << endl;
    file << "    {" << endl;

#if 0
yarp::dev::DeviceDescription d1;
d1.device_name = "left_arm_b1";
d1.device_type = "ControlBoard";
yarp::dev::DeviceDescription d2;
d2.device_name = "left_arm_b2";
d2.device_type = "ControlBoard";
yarp::dev::DeviceDescription d3;
d3.device_name = "left_arm_Remapper";
d3.device_type = "ControlBoardRemapper";
d3.device_extra_info = "left_arm_b1 left_arm_b2";
yarp::dev::DeviceDescription d4;
d4.device_name = "left_arm_NWS";
d4.device_type = "ControlBoard_nws_yarp";
d4.device_extra_info = "left_arm_Remapper";
devlist.push_back(d1);
devlist.push_back(d2);
devlist.push_back(d3);
devlist.push_back(d4);
#endif

    for (auto it = devlist.begin(); it != devlist.end(); it++)
    {
        string dname = it->device_name;
        string dtype = it->device_type;
        if (dtype.find("nws_yarp") != std::string::npos)
        {
            file << "    node_" << dname << "[label = <<FONT COLOR=\"red\" > " << dtype << "</FONT><BR/><FONT COLOR= \"black\"> " << dname << "</FONT> > shape=box style=filled fillcolor=lightblue];" << endl;
        }
        else
        {
            file << "    node_" << dname << "[label = <<FONT COLOR=\"red\" > " << dtype << "</FONT><BR/><FONT COLOR= \"black\"> " << dname << "</FONT> > shape=box];" << endl;
        }
    }
    file << endl;

    for (auto it = devlist.begin(); it != devlist.end(); it++)
    {
        string src = it->device_name;
        string extra = it->device_extra_info;
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream extrastream(extra);
        while (extrastream >> token) {tokens.push_back(token);}
        for (auto it2 = tokens.begin(); it2!=tokens.end(); it2++)
        {
            string dest = *it2;
            file << "    node_" << src << " -> " << "node_" << dest << endl;
        }
    }
    file << endl;

    file << "    label = yarprobotinterface"<< endl;
    file << "    }" << endl;
    file << "}" << endl << endl;
    file.close();

    return 1;
}

/********************
* dot file example
*********************

digraph G {

    splines="compound"

    rankdir="TB"
    subgraph cluster_net {

    node1 [label=<<FONT COLOR="red">ControlBoard_nws_yarp</FONT><BR/><FONT COLOR="black">left_arm</FONT>> shape=box];
    node_mot1 [label=<<FONT COLOR="red">Motor Control</FONT><BR/><FONT COLOR="black">left_arm_j0-1</FONT>> shape=box];
    node_mot2 [label=<<FONT COLOR="red">Motor Control</FONT><BR/><FONT COLOR="black">left_arm_j2-3</FONT>> shape=box];
    node_mot3 [label=<<FONT COLOR="red">Motor Control</FONT><BR/><FONT COLOR="black">left_arm_j4-j6</FONT>> shape=box];
    node_mot4 [label=<<FONT COLOR="red">Motor Control</FONT><BR/><FONT COLOR="black">left_arm_j7-j10</FONT>> shape=box];
    node_mot5 [label=<<FONT COLOR="red">Motor Control</FONT><BR/><FONT COLOR="black">left_arm_j11-j12</FONT>> shape=box];
    node100 [label=<<FONT COLOR="red">ControlBoard_nws_yarp</FONT><BR/><FONT COLOR="black">all_joints</FONT>> shape=box];
    node101 [label=<<FONT COLOR="red">ControlBoard_nws_ros2</FONT><BR/><FONT COLOR="black">all_joints</FONT>> shape=box];
    node_remap1 [label=<<FONT COLOR="red">ControlBoard_remapper</FONT><BR/><FONT COLOR="black">left_arm_remapper</FONT>> shape=box];
    node_remap1b [label=<<FONT COLOR="red">ControlBoard_remapper</FONT><BR/><FONT COLOR="black">left_arm_no_hand_remapper</FONT>> shape=box];
    node_remap2 [label=<<FONT COLOR="red">ControlBoard_remapper</FONT><BR/><FONT COLOR="black">all_joints</FONT>> shape=box];
    node_calib [label=<<FONT COLOR="red">Calibrator</FONT><BR/><FONT COLOR="black">left_arm_calibrator</FONT>> shape=box];

    node1 -> node_remap1
    node_remap1 -> node_mot1
    node_remap1 -> node_mot2
    node_remap1 -> node_mot3
    node_remap1 -> node_mot4
    node_remap1 -> node_mot5
    node_calib -> node_remap1
    node_remap1b -> node_mot1
    node_remap1b -> node_mot2
    node_remap1b -> node_mot3
    node_remap2 -> node_mot1
    node_remap2 -> node_mot2
    node_remap2 -> node_mot3
    node_remap2 -> node_mot4
    node_remap2 -> node_mot5
    node100 -> node_remap2
    node101 -> node_remap2

    label = yarprobotinterface
}
}
*/
