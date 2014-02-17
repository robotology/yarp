/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/node.h>


//using namespace ymm;

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


void Node::removeAllSuc(void)
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
