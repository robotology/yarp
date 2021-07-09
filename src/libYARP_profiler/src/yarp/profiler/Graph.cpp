/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/profiler/Graph.h>
#include <algorithm>
#include <stack>

//#include <sstream>
//#include <iostream>
using namespace yarp::profiler::graph;
using namespace yarp::os;


/**
 * yarp::profiler::graph::Edge
 */

Edge::Edge(const yarp::profiler::graph::Vertex& firstV,
           const yarp::profiler::graph::Vertex& secondV,
           yarp::os::Property property)
{
    firstVertex = &firstV;
    secondVertex = &secondV;
    Edge::property = property;
}

Edge::Edge(const Edge& edge)
{
    property = edge.property;
    firstVertex = edge.firstVertex;
    secondVertex = edge.secondVertex;
}

Edge::~Edge() = default;

const Vertex& Edge::first() const {
    return *firstVertex;
}

const Vertex& Edge::second() const {
    return *secondVertex;
}


bool Edge::operator == (const yarp::profiler::graph::Edge &edge) const {
    return (firstVertex == edge.firstVertex &&
            secondVertex == edge.secondVertex &&
            property.toString() == edge.property.toString());
}


/**
 * yarp::profiler::graph::Vertex
 */
Vertex::Vertex(const yarp::os::Property &prop) : property(prop) { }


Vertex::Vertex(const Vertex &vertex) = default;

Vertex::~Vertex() = default;

void Vertex::insertOuts(const yarp::profiler::graph::Edge& edge) {
    if( find(outs.begin(), outs.end(), edge) != outs.end()) return;
    outs.push_back(edge);
}

void Vertex::insertIns(const yarp::profiler::graph::Edge& edge) {
    if( find(ins.begin(), ins.end(), edge) != ins.end()) return;
    ins.push_back(edge);
}

/*
bool Vertex::operator == (const yarp::profiler::graph::Vertex &v1) const {
    return property.toString() == v1.property.toString();
}
*/

bool Vertex::operator<(const Vertex &v1) const {
    return property.toString() < v1.property.toString();
}


/**
 *   yarp::profiler::graph::Graph
 *
 */

Graph::Graph() = default;

/*
Graph::Graph(yarp::profiler::graph::Graph& graph) {
    mVertices == graph.mVertices;
}
*/

Graph::~Graph() {
    auto itr = mVertices.begin();
    for(;itr!=mVertices.end(); itr++) {
        Vertex* v = *itr;
        delete v;
    }
}

/*
void Graph::insert(Vertex *vertex) {
    yAssert(vertex != NULL);
    mVertices.push_back(vertex);
}
*/


pvertex_iterator Graph::insert(const Vertex &vertex){
    pvertex_iterator itr = find(vertex);
    if( itr != mVertices.end()) return itr;
   // Vertex* v = new Vertex(vertex);
    mVertices.push_back((Vertex*) &vertex);
    return mVertices.end()-1;
}


void Graph::remove(const Vertex &vertex){
    remove(find(vertex));
}

void Graph::remove(const pvertex_iterator vi) {
    if(vi == mVertices.end()) return;
    Vertex* v = *vi;
    mVertices.erase(vi);
    delete v;
}

void Graph::insertEdge(const Vertex &v1, const Vertex &v2, const yarp::os::Property &property) {
    insert(v1); // insert also checks for dubplication
    insert(v2);
    insertEdge(find(v1), find(v2), property);
}


void Graph::insertEdge(const pvertex_iterator vi1, const pvertex_iterator vi2,
                       const yarp::os::Property& property) {
    yAssert(vi1 != mVertices.end());
    yAssert(vi2 != mVertices.end());
    Edge edge(**vi1, **vi2, property);
    (**vi1).insertOuts(edge);
    (**vi2).insertIns(edge);
}

const pvertex_iterator Graph::find(const Vertex &vertex) {
    auto itr = mVertices.begin();
    for(;itr!=mVertices.end(); itr++) {
        if(*(*itr) == vertex)
            return itr;
    }
    return mVertices.end();

}

size_t Graph::size() {
    auto itr = mVertices.begin();
    size_t count = 0;
    for(; itr!=mVertices.end(); itr++)
        count += (**itr).degree();
    return count/2;
}

size_t Graph::nodesCount() {
    return mVertices.size();
}


void Graph::clear() {
    auto itr = mVertices.begin();
    for(; itr!=mVertices.end(); itr++)
        delete *itr;
    mVertices.clear();
}


void strongConnect(Vertex* v,
                   graph_subset& scc,
                   std::stack<Vertex*>&S, int& index) {
    //yDebug()<<"Visiting"<<v->property.find("name").asString()<<index;
    // Set the depth index for v to the smallest unused index
    v->property.put("index", index);
    v->property.put("lowlink", index);
    index++;
    S.push(v);
    v->property.put("onStack", 1);
    // Consider successors of v
    const edge_set& outs = v->outEdges();
    edge_const_iterator eitr;
    for(eitr = outs.begin(); eitr!=outs.end(); eitr++) {
        const Edge& e = (*eitr);
        const Vertex& w = e.second();
        //yDebug()<<"successors:"<<w.property.find("name").asString();
        if(!w.property.check("index")) {
            // Successor w has not yet been visited; recurse on it
            strongConnect((Vertex*)(&w), scc, S, index);
            int lowlink = std::min(v->property.find("lowlink").asInt32(),
                                   w.property.find("lowlink").asInt32());
            v->property.put("lowlink", lowlink);

        } else if (w.property.check("onStack")) {
            // Successor w is in stack S and hence in the current SCC
            int lowlink = std::min(v->property.find("lowlink").asInt32(),
                                   w.property.find("index").asInt32());
            v->property.put("lowlink", lowlink);
        }
    } // end successors

    // If v is a root node, pop the stack and generate an SCC
    if(v->property.find("lowlink").asInt32() == v->property.find("index").asInt32()) {
        // start a new strongly connected component
        pvertex_set vset;
        Vertex* w;
        do {
            w = S.top();
            S.pop();
            w->property.unput("onStack");
            //add w to current strongly connected component
            vset.push_back(w);
        } while(!S.empty() && w != v);
        //output the current strongly connected component
        if(vset.size() > 1) {
            scc.push_back(vset);
            //yInfo()<<"\nSCC:";
            //for(int i=0; i<vset.size(); i++)
            //    yInfo()<<vset[i]->property.find("name").asString();
        }
    }
}


bool Algorithm::calcSCC(yarp::profiler::graph::Graph& graph, graph_subset &scc) {
    scc.clear();

    // clear corresponding nodes propperties
    pvertex_const_iterator vitr;
    const pvertex_set& vertices = graph.vertices();
    for(vitr = vertices.begin(); vitr!=vertices.end(); vitr++) {
        Vertex* v = (*vitr);
        v->property.unput("onStack");
        v->property.unput("index");
        v->property.unput("lowlink");
    }

    std::stack<Vertex*> S;
    int index = 0;
    for(vitr = vertices.begin(); vitr!=vertices.end(); vitr++) {
        Vertex* v = (*vitr);
        if(!v->property.check("index"))
            strongConnect(v, scc, S, index);
    }
    return true;
}
