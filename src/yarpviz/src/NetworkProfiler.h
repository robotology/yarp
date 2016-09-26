/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
#ifndef NETWORK_PROFILER
#define NETWORK_PROFILER

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include<ggraph.h>
#include <yarp/os/Network.h>
#include<yarp/os/LogStream.h>
#include<yarp/os/Bottle.h>


class YarpvizVertex : public yarp::graph::Vertex {
public:
        YarpvizVertex(const yarp::os::Property &prop) : yarp::graph::Vertex(prop){
            graphicItem = NULL;
        }
        void setGraphicItem(void* item) { graphicItem= item; }
        void* getGraphicItem() { return graphicItem; }

private:
    void* graphicItem;
};


class PortVertex : public YarpvizVertex {
public:
        PortVertex(const std::string name) : YarpvizVertex("(type port)") , owner(NULL) {
            property.put("name", name);
        }

        void setOwner(yarp::graph::Vertex* owner) { PortVertex::owner = owner; }
        yarp::graph::Vertex* getOwner() { return owner; }

        virtual bool operator == (const yarp::graph::Vertex &v1) const {
            return property.find("name").asString() == v1.property.find("name").asString();
        }
private:
    yarp::graph::Vertex* owner;
};


class ProcessVertex : public YarpvizVertex{
public:
        ProcessVertex(int pid, const std::string hostname) : YarpvizVertex("(type process)") {
            property.put("hostname", hostname);
            property.put("pid", pid);
        }

        virtual bool operator == (const yarp::graph::Vertex &v1) const {
            return property.find("hostname").asString() == v1.property.find("hostname").asString() &&
                   property.find("pid").asInt() == v1.property.find("pid").asInt();
        }
};



class NetworkProfiler {

public:

    class ProgressCallback {
    public:
        virtual void onProgress(unsigned int percentage) { }
    };

    struct ConnectionInfo {
        std::string name;
        std::string carrier;
    };

    struct ProcessInfo {
        std::string name;
        std::string arguments;
        std::string os;
        std::string hostname;
        int pid;
        int priority;
        int policy;
        ProcessInfo() { pid = priority = policy = -1; }
    };

    struct PortDetails {
        std::string name;
        std::vector<ConnectionInfo> outputs;
        std::vector<ConnectionInfo> inputs;
        ProcessInfo owner;
        std::string toString() {
            std::ostringstream str;
            str<<"port name: "<<name<<std::endl;
            str<<"outputs:"<<std::endl;
            std::vector<ConnectionInfo>::iterator itr;
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
    static bool yarpNameList(ports_name_set& ports);

    /**
     * @brief creatNetworkGraph
     * @param details
     * @param graph
     * @return
     */
    static bool creatNetworkGraph(ports_detail_set details, yarp::graph::Graph& graph);

    static bool creatSimpleModuleGraph(yarp::graph::Graph& graph, yarp::graph::Graph& subgraph);

    /**
     * @brief NetworkProfiler::yarpClean
     * @param timeout
     * @return
     */
    static bool yarpClean(float timeout=0.1);

    static void setProgressCallback(ProgressCallback* callback) {
        progCallback = callback;
    }

    static bool updateConnectionQosStatus(yarp::graph::Graph& graph);

    static std::string packetPrioToString(yarp::os::QosStyle::PacketPriorityLevel level);
    static yarp::os::QosStyle::PacketPriorityLevel packetStringToPrio(std::string level);

    static bool attachPortmonitorPlugin(std::string portName, yarp::os::Property pluginProp);
    static bool detachPortmonitorPlugin(std::string portName);

private:
        static ProgressCallback* progCallback;

};


#endif // NETWORK_PROFILER
