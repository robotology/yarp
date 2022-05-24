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

#include <yarp/profiler/NetworkProfilerBasic.h>
#include <yarp/profiler/Graph.h>
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Bottle.h>


namespace yarp::profiler {

class NetworkProfiler : public NetworkProfilerBasic
{
public:

    class ProgressCallback
    {
    public:
        virtual ~ProgressCallback() { }
        virtual void onProgress(unsigned int percentage) { }
    };

    /**
     * @brief creatNetworkGraph
     * @param details
     * @param graph
     * @return
     */
    static bool creatNetworkGraph(ports_detail_set details, yarp::profiler::graph::Graph& graph);

    static bool creatSimpleModuleGraph(yarp::profiler::graph::Graph& graph, yarp::profiler::graph::Graph& subgraph);

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
