/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_PROFILER_NETWORK_PROFILER_H
#define YARP_PROFILER_NETWORK_PROFILER_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include <yarp/profiler/Graph.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>


namespace yarp {
    namespace profiler {

class NetworkProfiler
{

public:

    class ProgressCallback
    {
    public:
        virtual ~ProgressCallback() { }
        virtual void onProgress(unsigned int percentage) { }
    };

    struct ConnectionInfo
    {
        std::string name;
        std::string carrier;
    };

    struct MachineInfo
    {
        std::string os;
        std::string hostname;

    };

    struct ProcessInfo
    {
        std::string name;
        std::string arguments;
        std::string os;
        std::string hostname;
        MachineInfo owner;
        int pid;
        int priority;
        int policy;
        ProcessInfo() { pid = priority = policy = -1; }
    };

    struct PortDetails
    {
        std::string name;
        std::vector<ConnectionInfo> outputs;
        std::vector<ConnectionInfo> inputs;
        ProcessInfo owner;
        std::string toString() const {
            std::ostringstream str;
            str<<"port name: "<<name<<std::endl;
            str<<"outputs:"<<std::endl;
            std::vector<ConnectionInfo>::const_iterator itr;
            for(itr=outputs.begin(); itr!=outputs.end(); itr++)
                str<<"   + "<<(*itr).name<<" ("<<(*itr).carrier<<")"<<std::endl;
            str<<"inputs:"<<std::endl;
            for(itr=inputs.begin(); itr!=inputs.end(); itr++)
                str<<"   + "<<(*itr).name<<" ("<<(*itr).carrier<<")"<<std::endl;
            str<<"owner:"<<std::endl;
            str<<"   + name:      "<<owner.name<<std::endl;
            str<<"   + arguments: "<<owner.arguments<<std::endl;
            str<<"   + hostname:  "<<owner.hostname<<std::endl;
            str<<"   + priority:  "<<owner.priority<<std::endl;
            str<<"   + policy:    "<<owner.policy<<std::endl;
            str<<"   + os:        "<<owner.os<<std::endl;
            str<<"   + pid:       "<<owner.pid<<std::endl;
            return str.str();
        }
    };

    typedef  std::vector<yarp::os::Bottle> ports_name_set;
    typedef  ports_name_set::iterator ports_name_iterator;

    typedef  std::vector<PortDetails> ports_detail_set;
    typedef  ports_detail_set::iterator ports_detail_iterator;

public:
    /**
     * @brief getPortDetails
     * @param portName
     * @param info
     * @return
     */
    static bool getPortDetails(const std::string& portName, PortDetails& info);

    /**
     * @brief yarpNameList
     * @param ports
     * @return
     */
    static bool yarpNameList(ports_name_set& ports, bool complete=false);

    /**
     * @brief creatNetworkGraph
     * @param details
     * @param graph
     * @return
     */
    static bool creatNetworkGraph(ports_detail_set details, yarp::profiler::graph::Graph& graph);

    static bool creatSimpleModuleGraph(yarp::profiler::graph::Graph& graph, yarp::profiler::graph::Graph& subgraph);

    /**
     * @brief NetworkProfiler::yarpClean
     * @param timeout
     * @return
     */
    static bool yarpClean(float timeout=0.1);

    static void setProgressCallback(ProgressCallback* callback) {
        progCallback = callback;
    }

    static bool updateConnectionQosStatus(yarp::profiler::graph::Graph& graph);

    static std::string packetPrioToString(yarp::os::QosStyle::PacketPriorityLevel level);
    static yarp::os::QosStyle::PacketPriorityLevel packetStringToPrio(std::string level);

    static bool attachPortmonitorPlugin(std::string portName, yarp::os::Property pluginProp);
    static bool detachPortmonitorPlugin(std::string portName);
    static bool setPortmonitorParams(std::string portName, yarp::os::Property& param);
    static bool getPortmonitorParams(std::string portName, yarp::os::Bottle &param);

private:
        static ProgressCallback* progCallback;

};

    }
}


#endif //YARP_PROFILER_NETWORK_PROFILER_H
