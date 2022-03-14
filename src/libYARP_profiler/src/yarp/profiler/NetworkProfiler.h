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


namespace yarp::profiler {

class NetworkProfiler
{

public:

    class ProgressCallback
    {
    public:
        virtual ~ProgressCallback() { }
        virtual void onProgress(unsigned int percentage) { }
    };

    struct ConnectedPortInfo
    {
        std::string port_name;
        std::string carrier;
    };

    struct PortInfo
    {
        std::string name;
        std::string ip;
        std::string port_number;
    };

    struct  ConnectionDetails
    {
        PortInfo src;
        PortInfo dst;
        std::string carrier;
        bool operator==(const ConnectionDetails& rhs)
        {
            return (this->src.name == rhs.src.name &&
                    this->dst.name == rhs.dst.name &&
                    this->carrier == rhs.carrier);
        }
        bool isValid()
        {
            if (src.name != "" &&
                dst.name != "" &&
                carrier != "") return true;
            return false;
        }
    };

    struct MachineInfo
    {
        std::string os;
        std::string hostname;
        std::string ip;
    };

    struct ProcessInfo
    {
        MachineInfo owner_machine;
        std::string process_name;
        std::string arguments;
        std::string process_fullname;
        int pid;
        int priority;
        int policy;
        ProcessInfo() { pid = priority = policy = -1; }
    };

    struct PortDetails
    {
        PortInfo info;
        std::vector<ConnectedPortInfo> outputs;
        std::vector<ConnectedPortInfo> inputs;
        ProcessInfo owner_process;

        std::string toString() const;
    };

    typedef  std::vector<PortInfo> ports_name_set;
    typedef  ports_name_set::iterator ports_name_iterator;

    typedef  std::vector<PortDetails> ports_detail_set;
    typedef  ports_detail_set::iterator ports_detail_iterator;

    typedef  std::vector<ConnectionDetails> connections_set;

    typedef  std::vector<std::string> machines_list;
    typedef  std::vector<std::string> processes_list;

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
    static bool getMachinesList (const ports_detail_set& ports, machines_list& l);
    static bool getProcessesList(const ports_detail_set& ports, processes_list& l);

    static bool getPortsList(ports_name_set& ports, bool complete=false);
    static bool getPortsDetailedList(ports_detail_set& ports, bool complete = false);
    static bool getConnectionsList(connections_set& connections);
    static bool getPortInfo(const std::string& name, const ports_name_set& ports, PortInfo& p);

    static void filterConnectionListByName(const connections_set& in, connections_set& filtered_out, std::string src_name="*", std::string dst_name="*");
    static void filterConnectionListByIp(const connections_set& in, connections_set& filtered_out, std::string src_name = "*", std::string dst_name = "*");
    static void filterConnectionListByPortNumber(const connections_set& in, connections_set& filtered_out, std::string src_name = "*", std::string dst_name = "*");

    static void filterPortsListByIp(const ports_detail_set& in, ports_detail_set& filtered_out, std::string ip = "*");
    static void filterPortsListByProcess(const ports_detail_set& in, ports_detail_set& filtered_out, std::string fullprocess = "*");


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

} // namespace yarp::profiler


#endif //YARP_PROFILER_NETWORK_PROFILER_H
