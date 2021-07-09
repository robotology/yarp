/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_GRAPH
#define YARP_MANAGER_GRAPH

#include <map>
#include <string>
#include <iterator>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>


namespace yarp {
namespace manager {

typedef std::map<std::string, Node*> NodePContainer;
typedef std::map<std::string, Node*>::iterator NodePIterator;

class GraphIterator;

/**
 * Class Graph
 */
class Graph {

public:
    Graph();
    virtual ~Graph();

    int getSize() { return nodes.size(); }
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
    void clear();
    void setSatisfied(bool sat);
    void setVisited(bool vis);
    bool hasNode(Node* node);
    bool hasNode(const char* szLabel);
    Node* getNode( const char* szLabel);
    GraphIterator begin();
    GraphIterator end();

protected:

private:
    NodePContainer nodes;
    NodePIterator findNode(Node* node);
};


/**
 *  Class GraphIterator
 */
class GraphIterator: public std::iterator<std::input_iterator_tag, Node*>
{
public:
    GraphIterator() = default;
    virtual ~GraphIterator() = default;
    GraphIterator& operator++() {++itr;return *this;}
    GraphIterator operator++(int) {GraphIterator tmp(*this); operator++(); return tmp;}
    bool operator==(const GraphIterator& rhs) const {return itr==rhs.itr;}
    bool operator!=(const GraphIterator& rhs) const {return itr!=rhs.itr;}
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
            printf("\033[31m%s\033[0m, ", ((vertex*)list_get_at(&l.nodes, i))->label);\
    else\
            printf("%s, ", ((vertex*)list_get_at(&l.nodes, i))->label);\
*/

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_GRAPH__
