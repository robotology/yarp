// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __YARP_MANAGER_NODE__
#define __YARP_MANAGER_NODE__

#include <iostream>
#include <vector>
#include <string>

#include <yarp/manager/ymm-types.h>

using namespace std;

namespace yarp {
namespace manager {

class Node;
class Link;

typedef vector<Link> LinkContainer;
typedef vector<Link>::iterator LinkIterator;

class GraphicModel {

public:
    GraphicModel(void) {}
    virtual ~GraphicModel() {}
    vector<GyPoint> points;
};



/**
 * Link holding all the links of a node
 */
class Link {

public:
    Link(Node* to, float weight, bool virtualLink=false) {
         fWeight = weight;
         connectTo = to;
         bVirtual = virtualLink;
         }
    virtual ~Link(){}
    Node* to() { return connectTo; }
    void setWeight(float w) { fWeight = w; }
    float weight(void) { return fWeight; }
    void setVirtual(bool virtualLink) { bVirtual = virtualLink;}
    bool isVirtual(void){ return bVirtual; }
protected:

private:
    bool bVirtual;
    float fWeight;
    Node* connectTo;
};



/**
 * a Node of a Graph
 */
class Node {

public:
    Node(NodeType _type){
        type = _type;
        bSatisfied = false;
        bVisited = false;
        model = NULL;
    }
    Node(NodeType _type, const char* szLabel ){
        type = _type;
        bSatisfied = false;
        bVisited = false;
        model = NULL;
        if(szLabel) label = szLabel;
    }
    Node(const Node &node);
    virtual ~Node() { model = NULL; }

    void setSatisfied(bool sat) { bSatisfied = sat; }
    bool isSatisfied(void) { return bSatisfied; }
    void setVisited(bool vis) { bVisited = vis; }
    bool isVisited(void) { return bVisited; }
    bool isLeaf(void) {
        return ((sucCount()==0) ? true : false);
    }

    NodeType getType(void) { return type; }
    void setLabel(const char* szLabel) { if(szLabel) label = szLabel; }
    const char* getLabel(void) { return label.c_str(); }
    int sucCount(void) { return (int)sucessors.size(); }
    Link &getLinkAt(int index) { return sucessors[index]; }


    bool addSuc(Node* node, float weight, bool _virtual=false);
    bool removeSuc(Node* node);
    void removeAllSuc(void);
    bool hasSuc(Node* node);
    virtual Node* clone() = 0;

    GraphicModel* getModel(void) { return model;}
    void setModel(GraphicModel* mdl) { model = mdl; };


protected:

private:
    LinkIterator findSuc(Node* node);
    LinkContainer sucessors;
    bool bSatisfied;
    bool bVisited;
    NodeType type;
    string label;
    GraphicModel* model;
};

typedef vector<Node*> NodePVector;
typedef vector<Node*>::iterator NodePVIterator;

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_NODE_
