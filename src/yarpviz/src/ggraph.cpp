/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include<yarp/os/Log.h>
#include<yarp/os/LogStream.h>
#include <ggraph.h>
#include <algorithm>

//#include <sstream>
//#include <iostream>
using namespace yarp::graph;
using namespace yarp::os;

#define myDebug yDebug

/**
 * yarp::graph::Edge
 */

Edge::Edge(const yarp::graph::Vertex& firstV,
           const yarp::graph::Vertex& secondV,
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

Edge::~Edge() { }

const Vertex& Edge::first() const {
    return *firstVertex;
}

const Vertex& Edge::second() const {
    return *secondVertex;
}


bool Edge::operator == (const yarp::graph::Edge &edge) const {
    return (firstVertex == edge.firstVertex &&
            secondVertex == edge.secondVertex);
}


/**
 * yarp::graph::Vertex
 */
Vertex::Vertex(const yarp::os::Property &prop) : property(prop) { }


Vertex::Vertex(const Vertex &vertex)
    : property(vertex.property),
      outs(vertex.outs),
      ins(vertex.ins) { }

Vertex::~Vertex() { }

void Vertex::insertOuts(const yarp::graph::Edge& edge) {
    if( find(outs.begin(), outs.end(), edge) != outs.end()) return;
    outs.push_back(edge);
}

void Vertex::insertIns(const yarp::graph::Edge& edge) {
    if( find(ins.begin(), ins.end(), edge) != ins.end()) return;
    ins.push_back(edge);
}

/*
bool Vertex::operator == (const yarp::graph::Vertex &v1) const {
    return property.toString() == v1.property.toString();
}
*/

bool Vertex::operator<(const Vertex &v1) const {
    return property.toString() < v1.property.toString();
}


/**
 *   yarp::graph::Graph
 *
 */

Graph::Graph() {
}

/*
Graph::Graph(yarp::graph::Graph& graph) {
    mVertices == graph.mVertices;
}
*/

Graph::~Graph() {
    pvertex_iterator itr = mVertices.begin();
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


void Graph::insert(const Vertex &vertex){
    if( find(vertex) != mVertices.end()) return;
   // Vertex* v = new Vertex(vertex);
    mVertices.push_back((Vertex*) &vertex);
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
    pvertex_iterator itr = mVertices.begin();
    for(;itr!=mVertices.end(); itr++) {
        if(*(*itr) == vertex)
            return itr;
    }
    return mVertices.end();

}

size_t Graph::size() {
    pvertex_iterator itr = mVertices.begin();
    size_t count = 0;
    for(; itr!=mVertices.end(); itr++)
        count += (**itr).degree();
    return count/2;
}

size_t Graph::nodesCount() {
    return mVertices.size();
}


void Graph::clear() {
    pvertex_iterator itr = mVertices.begin();
    size_t count = 0;
    for(; itr!=mVertices.end(); itr++)
        delete *itr;
    mVertices.clear();
}
