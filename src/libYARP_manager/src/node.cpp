/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/manager/node.h>


using namespace yarp::manager;

/**
 * class Node
 */

Node::Node(const Node &node)
{
    bVisited = node.bVisited;
    bSatisfied = node.bSatisfied;
    type = node.type;
    label = node.label;
    sucessors = node.sucessors;
    model = node.model;
}

bool Node::addSuc(Node* node, float weight, bool _virtual)
{
    __CHECK_NULLPTR(node);

    if(!hasSuc(node))
    {
        Link ln(node, weight, _virtual);
        sucessors.push_back(ln);
    }
    return true;
}


bool Node::removeSuc(Node* node)
{
    __CHECK_NULLPTR(node);

    LinkIterator it = findSuc(node);
    if(it != sucessors.end())
        sucessors.erase(it);
    return true;
}


void Node::removeAllSuc()
{
    sucessors.clear();
}



bool Node::hasSuc(Node* node)
{
    LinkIterator it = findSuc(node);
    if(it == sucessors.end())
        return false;
    return true;
}



LinkIterator Node::findSuc(Node* node)
{
    LinkIterator itr;
    for(itr=sucessors.begin(); itr<sucessors.end(); itr++)
        if ((*itr).to() == node)
            return itr;
    return sucessors.end();
}
