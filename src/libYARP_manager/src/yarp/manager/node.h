/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_NODE
#define YARP_MANAGER_NODE

#include <iostream>
#include <vector>
#include <string>

#include <yarp/manager/ymm-types.h>


namespace yarp {
namespace manager {

class Node;
class Link;

typedef std::vector<Link> LinkContainer;
typedef std::vector<Link>::iterator LinkIterator;

class GraphicModel {

public:
    GraphicModel() = default;
    virtual ~GraphicModel() = default;
    std::vector<GyPoint> points;
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
    float weight() { return fWeight; }
    void setVirtual(bool virtualLink) { bVirtual = virtualLink;}
    bool isVirtual(){ return bVirtual; }
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
        model = nullptr;
    }
    Node(NodeType _type, const char* szLabel ){
        type = _type;
        bSatisfied = false;
        bVisited = false;
        model = nullptr;
        if(szLabel) { label = szLabel; }
    }

    virtual ~Node() { model = nullptr; }

    void setSatisfied(bool sat) { bSatisfied = sat; }
    bool isSatisfied() { return bSatisfied; }
    void setVisited(bool vis) { bVisited = vis; }
    bool isVisited() { return bVisited; }
    bool isLeaf() {
        return ((sucCount()==0) ? true : false);
    }

    NodeType getType() { return type; }
    void setLabel(const char* szLabel) { if(szLabel) { label = szLabel; } }
    const char* getLabel() { return label.c_str(); }
    int sucCount() { return static_cast<int>(sucessors.size()); }
    Link &getLinkAt(int index) { return sucessors[index]; }


    bool addSuc(Node* node, float weight, bool _virtual=false);
    bool removeSuc(Node* node);
    void removeAllSuc();
    bool hasSuc(Node* node);
    virtual Node* clone() = 0;

    GraphicModel* getModel() { return model;}
    void setModel(GraphicModel* mdl) { model = mdl; };


protected:

private:
    LinkIterator findSuc(Node* node);
    LinkContainer sucessors;
    bool bSatisfied;
    bool bVisited;
    NodeType type;
    std::string label;
    GraphicModel* model;
};

typedef std::vector<Node*> NodePVector;
typedef std::vector<Node*>::iterator NodePVIterator;

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_NODE_
