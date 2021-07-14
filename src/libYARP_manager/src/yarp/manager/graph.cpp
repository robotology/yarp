/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/graph.h>

using namespace yarp::manager;

Graph::Graph() = default;

Graph::~Graph()
{
    clear();
}

Node* Graph::addNode(Node* _node)
{
    //__CHECK_NULLPTR(_node);
    if (!_node) {
        return nullptr;
    }
    if (hasNode(_node)) {
        return nullptr;
    }

    Node* node = _node->clone();
    nodes[node->getLabel()] = node;
    return node;
}

bool Graph::removeNode(Node* node)
{
    __CHECK_NULLPTR(node);

    auto itr = nodes.find(node->getLabel());
    if (itr == nodes.end()) {
        return true;
    }
    delete (*itr).second;
    nodes.erase(itr);
    return true;
}


bool Graph::removeNode(const char* szLabel)
{
    auto itr = nodes.find(szLabel);
    if (itr == nodes.end()) {
        return true;
    }
    delete (*itr).second;
    nodes.erase(itr);
    return true;
}

void Graph::clear()
{
    NodePIterator itr;
    for (itr = nodes.begin(); itr != nodes.end(); itr++) {
        delete ((*itr).second);
    }
    nodes.clear();
}

void Graph::setSatisfied(bool sat)
{
    NodePIterator itr;
    for (itr = nodes.begin(); itr != nodes.end(); itr++) {
        ((*itr).second)->setSatisfied(sat);
    }
}

void Graph::setVisited(bool vis)
{
    NodePIterator itr;
    for (itr = nodes.begin(); itr != nodes.end(); itr++) {
        ((*itr).second)->setVisited(vis);
    }
}

Node* Graph::getNode( const char* szLabel)
{
    auto itr = nodes.find(szLabel);
    if (itr != nodes.end()) {
        return (*itr).second;
    }
    return nullptr;
}

bool Graph::addLink(Node* first, Node* second,
                    float weight, bool _virtual)
{
    __CHECK_NULLPTR(first);
    __CHECK_NULLPTR(second);

    first->addSuc(second, weight, _virtual);
    return true;
}

bool Graph::addLink(const char* szFirst, const char* szSecond,
                    float weight, bool _virtual)
{
    Node* first = getNode(szFirst);
    Node* second  = getNode(szSecond);
    __CHECK_NULLPTR(first);
    __CHECK_NULLPTR(second);

    first->addSuc(second, weight, _virtual);
    return true;
}


bool Graph::removeLink(Node* first, Node* second)
{
    __CHECK_NULLPTR(first);
    __CHECK_NULLPTR(second);

    first->removeSuc(second);
    return true;
}

bool Graph::removeLink(const char* szFirst, const char* szSecond)
{
    Node* first = getNode(szFirst);
    Node* second  = getNode(szSecond);
    __CHECK_NULLPTR(first);
    __CHECK_NULLPTR(second);

    first->removeSuc(second);
    return true;
}


bool Graph::hasNode(Node* node)
{
    __CHECK_NULLPTR(node);

    auto itr = nodes.find(node->getLabel());
    if (itr == nodes.end()) {
        return false;
    }
    return true;
}

bool Graph::hasNode(const char* szLabel)
{
    if (getNode(szLabel)) {
        return true;
    }
    return false;
}


Node* Graph::getNodeAt(int index)
{
    auto itr = nodes.begin();
    for (int i = 0; i < index; i++) {
        itr++;
    }
    return (*itr).second;
}


GraphIterator Graph::begin()
{
    GraphIterator itr;
    itr.itr = nodes.begin();
    return itr;
}

GraphIterator Graph::end()
{
    GraphIterator itr;
    itr.itr = nodes.end();
    return itr;
}

NodePIterator Graph::findNode(Node* node)
{
    NodePIterator itr;
    for (itr = nodes.begin(); itr != nodes.end(); itr++) {
        if ((*itr).second == node) {
            return itr;
        }
    }
    return nodes.end();
}
