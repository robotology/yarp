/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "graph.h"

Graph::Graph(void) { }

Graph::~Graph()
{
    clear();
}

Node* Graph::addNode(Node* _node)
{
    //__CHECK_NULLPTR(_node);
    if(!_node)
        return NULL;
    if(hasNode(_node))
        return NULL;

    Node* node = _node->clone();
    nodes[node->getLabel()] = node;
    return node;
}

bool Graph::removeNode(Node* node)
{
    __CHECK_NULLPTR(node);

    NodePIterator itr = nodes.find(node->getLabel());
    if(itr == nodes.end())
        return true;
    delete (*itr).second;
    nodes.erase(itr);
    return true;
}


bool Graph::removeNode(const char* szLabel)
{
    NodePIterator itr = nodes.find(szLabel);
    if(itr == nodes.end())
        return true;
    delete (*itr).second;
    nodes.erase(itr);
    return true;
}

void Graph::clear(void)
{
    NodePIterator itr;
    for(itr=nodes.begin(); itr!=nodes.end(); itr++)
        delete ((*itr).second);
    nodes.clear();
}

void Graph::setSatisfied(bool sat)
{
    NodePIterator itr;
    for(itr=nodes.begin(); itr!=nodes.end(); itr++)
        ((*itr).second)->setSatisfied(sat);
}

void Graph::setVisited(bool vis)
{
    NodePIterator itr;
    for(itr=nodes.begin(); itr!=nodes.end(); itr++)
        ((*itr).second)->setVisited(vis);
}

Node* Graph::getNode( const char* szLabel)
{
    NodePIterator itr = nodes.find(szLabel);
    if(itr != nodes.end())
        return (*itr).second;
    return NULL;
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

    NodePIterator itr = nodes.find(node->getLabel());
    if(itr == nodes.end())
        return false;
    return true;
}

bool Graph::hasNode(const char* szLabel)
{
    if(getNode(szLabel))
        return true;
    return false;
}


Node* Graph::getNodeAt(int index)
{
    NodePIterator itr = nodes.begin();
    for(int i=0; i<index; i++)
        itr++;
    return (*itr).second;
}


GraphIterator Graph::begin(void)
{
    GraphIterator itr;
    itr.itr = nodes.begin();
    return itr;
}

GraphIterator Graph::end(void)
{
    GraphIterator itr;
    itr.itr = nodes.end();
    return itr;
}

NodePIterator Graph::findNode(Node* node)
{
    NodePIterator itr;
    for(itr=nodes.begin(); itr!=nodes.end(); itr++)
        if ((*itr).second == node)
            return itr;
    return nodes.end();
}
