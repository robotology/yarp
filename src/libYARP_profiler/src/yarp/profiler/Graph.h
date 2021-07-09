/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_PROFILER_GRAP_H
#define YARP_PROFILER_GRAP_H

#include<yarp/os/Property.h>

#include <iostream>
#include <vector>
#include <string>

namespace yarp {
    namespace profiler {
        namespace graph {
            class Vertex;
            class Edge;
            class Graph;
            class Algorithm;
            class GraphicVertex;
            class OwnedVertex;
            class ProcessVertex;
            class PortVertex;
            class MachineVertex;

        }
    }
}


typedef  std::vector<yarp::profiler::graph::Edge> edge_set;
typedef  edge_set::iterator edge_iterator;
typedef  edge_set::const_iterator edge_const_iterator;

typedef  std::vector<yarp::profiler::graph::Vertex*> pvertex_set;
typedef  pvertex_set::iterator pvertex_iterator;
typedef  pvertex_set::const_iterator pvertex_const_iterator;

typedef  std::vector<pvertex_set> graph_subset;
typedef  graph_subset::iterator graph_subset_iterator;
typedef  graph_subset::const_iterator graph_subset_const_iterator;


/**
 * @brief The yarp::profiler::graph::Edge class
 */
class yarp::profiler::graph::Edge {
public:

    Edge(const yarp::profiler::graph::Vertex& firstV,
         const yarp::profiler::graph::Vertex& secondV,
         yarp::os::Property property="");

    Edge(const Edge& edge);

    virtual ~Edge();

    const yarp::profiler::graph::Vertex& first() const;
    const yarp::profiler::graph::Vertex& second() const;
    virtual bool operator == (const yarp::profiler::graph::Edge &edge) const;

public:
    yarp::os::Property property;

private:
    const yarp::profiler::graph::Vertex* firstVertex;
    const yarp::profiler::graph::Vertex* secondVertex;
};


/**
 * @brief The yarp::profiler::graph::Vertex class
 */
class yarp::profiler::graph::Vertex {

public:
    Vertex(const yarp::os::Property &prop);
    Vertex(const yarp::profiler::graph::Vertex& vertex);
    virtual ~Vertex();

    const edge_set& outEdges() const { return outs; }
    const edge_set& inEdges() const { return ins; }
    size_t degree() const { return ins.size() + outs.size(); }

    virtual bool operator == (const yarp::profiler::graph::Vertex &v1) const = 0;
    virtual bool operator<(const Vertex &v1) const;

    friend class Graph;

public:
    yarp::os::Property property;

private:
    void insertOuts(const yarp::profiler::graph::Edge& edge);
    void insertIns(const yarp::profiler::graph::Edge& edge);

private:
    edge_set outs;
    edge_set ins;
};


/**
 * @brief The yarp::profiler::graph::Graph class
 */
class yarp::profiler::graph::Graph {

public:
    Graph();
    //Graph(yarp::profiler::graph::Graph& graph);
    virtual ~Graph();

    //void insert(Vertex *vertex);
    pvertex_iterator insert(const Vertex &vertex);
    void remove(const Vertex &vertex);
    void remove(const pvertex_iterator vi);

    void insertEdge(const Vertex &v1, const Vertex &v2,
                    const yarp::os::Property &property="");

    void insertEdge(const pvertex_iterator vi1, const pvertex_iterator vi2,
                    const yarp::os::Property &property="");

    const pvertex_iterator find(const Vertex &v1);

    size_t size();
    size_t nodesCount();
    const pvertex_set& vertices() { return mVertices; }
    size_t order() { return mVertices.size(); }

    void clear();

private:
    pvertex_set mVertices;
};


class yarp::profiler::graph::Algorithm {
public:
    /**
     * @brief calcSCC
     * @param graph
     * @return
     */
    static bool calcSCC(yarp::profiler::graph::Graph& graph, graph_subset &scc);
};

class yarp::profiler::graph::GraphicVertex : public yarp::profiler::graph::Vertex
{
public:
        GraphicVertex(const yarp::os::Property &prop) : yarp::profiler::graph::Vertex(prop){
            graphicItem = nullptr;
        }
        void setGraphicItem(void* item) { graphicItem= item; }
        void* getGraphicItem() { return graphicItem; }

private:
    void* graphicItem;
};

class yarp::profiler::graph::OwnedVertex :  public yarp::profiler::graph::GraphicVertex
{
public:
    OwnedVertex(const yarp::os::Property &prop) : yarp::profiler::graph::GraphicVertex(prop)
    {
        owner = nullptr;
    }
    bool setOwner(yarp::profiler::graph::Vertex* _owner) {
        if (_owner)
        {
            owner = _owner;
            return true;
        }
        return false;
    }
    yarp::profiler::graph::Vertex* getOwner() { return owner; }
private:
    yarp::profiler::graph::Vertex* owner;

};

class yarp::profiler::graph::PortVertex : public OwnedVertex
{
public:
        PortVertex(const std::string name) : yarp::profiler::graph::OwnedVertex("(type port)") {
            property.put("name", name);
        }
        virtual ~PortVertex(){}

        bool operator == (const yarp::profiler::graph::Vertex &v1) const override {
            return property.find("name").asString() == v1.property.find("name").asString();
        }

};

class yarp::profiler::graph::ProcessVertex : public yarp::profiler::graph::OwnedVertex
{
public:
        ProcessVertex(int pid, const std::string hostname) : yarp::profiler::graph::OwnedVertex("(type process)") {
            property.put("hostname", hostname);
            property.put("pid", pid);
        }
        virtual ~ProcessVertex(){}

        bool operator == (const yarp::profiler::graph::Vertex &v1) const override {
            return property.find("hostname").asString() == v1.property.find("hostname").asString() &&
                   property.find("pid").asInt32() == v1.property.find("pid").asInt32();
        }

};

class yarp::profiler::graph::MachineVertex : public yarp::profiler::graph::GraphicVertex
{
public:
        MachineVertex(std::string os, const std::string hostname) : yarp::profiler::graph::GraphicVertex("(type machine)") {
            property.put("hostname", hostname);
            property.put("os", os);
        }
        virtual ~MachineVertex() {}

        bool operator == (const yarp::profiler::graph::Vertex &v1) const override {
            return property.find("hostname").asString() == v1.property.find("hostname").asString() &&
                   property.find("os").asString() == v1.property.find("os").asString() &&
                   property.find("type").asString() == v1.property.find("type").asString() ;
        }
};


#endif // YARP_PROFILER_GRAP_H
