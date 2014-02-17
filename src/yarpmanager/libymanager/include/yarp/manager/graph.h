// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef __YARP_MANAGER_GRAPH__
#define __YARP_MANAGER_GRAPH__

#include <map>
#include <string>
#include <iterator>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>

using namespace std;

namespace yarp {
namespace manager {

typedef map<string, Node*> NodePContainer;
typedef map<string, Node*>::iterator NodePIterator;

class GraphIterator;

/**
 * Class Graph
 */
class Graph {

public:
    Graph(void);
    virtual ~Graph();

    int getSize(void) { return nodes.size(); }
    Node* getNodeAt(int index);

    Node* addNode(Node* node);
    bool removeNode(Node* node);
    bool removeNode(const char* szLabel);
    bool addLink(Node* first, Node* second,
                float weight, bool _virtual=false);
    bool addLink(const char* szFirst, const char* szSecond,
                float weight, bool _virtual=false);
    bool removeLink(Node* first, Node* second);
    bool removeLink(const char* szFirst, const char* szSecond);
    void clear(void);
    void setSatisfied(bool sat);
    void setVisited(bool vis);
    bool hasNode(Node* node);
    bool hasNode(const char* szLabel);
    Node* getNode( const char* szLabel);
    GraphIterator begin(void);
    GraphIterator end(void);

protected:

private:
    NodePContainer nodes;
    NodePIterator findNode(Node* node);
};


/**
 *  Class GraphIterator
 */
class GraphIterator: public iterator<input_iterator_tag, Node*>
{
public:
    GraphIterator(void){}
    virtual ~GraphIterator(){}
    GraphIterator(const GraphIterator& mit) : itr(mit.itr) {}
    GraphIterator& operator++() {++itr;return *this;}
    GraphIterator operator++(int) {GraphIterator tmp(*this); operator++(); return tmp;}
    bool operator==(const GraphIterator& rhs) {return itr==rhs.itr;}
    bool operator!=(const GraphIterator& rhs) {return itr!=rhs.itr;}
    Node*& operator*() {return (*itr).second;}
    friend class Graph;

private:
    NodePIterator itr;
};


#define PRINT_GRAPH(g)\
        cout<<"Graph "<<#g<<" with "<<g.getSize()<<" nodes:"<<endl;\
        cout<<"{"<<endl;\
        for(GraphIterator itr=g.begin(); itr!=g.end(); itr++)\
        {\
            Node* n = (*itr);\
            cout<<" "<<n->getLabel()<<": [";\
            for(int j=0; j<n->sucCount(); j++)\
            {\
                Link l = n->getLinkAt(j);\
                cout<<l.to()->getLabel()<<"("<<l.weight()<<"), ";\
            }\
            cout<<"]"<<endl;\
        }\
        cout<<"}"<<endl;


/*                if( ! *((bool*)list_get_at(&l.marks, i)) )\
            printf("\033[31m%s\033[0m, ", ((vertex*)list_get_at(&l.nodes, i))->lable);\
    else\
            printf("%s, ", ((vertex*)list_get_at(&l.nodes, i))->lable);\
*/

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_GRAPH__
