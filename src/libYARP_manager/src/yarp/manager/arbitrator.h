/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_ARBITRATOR
#define YARP_MANAGER_ARBITRATOR

#include <map>
#include <vector>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/node.h>
#include <yarp/manager/utility.h>


namespace yarp {
namespace manager {


/**
 * Class port Arbitrator
 */

class Arbitrator {
public:
    Arbitrator() {
        model = NULL;
        appOwner = NULL;
    }
    Arbitrator(const char* szPort) {
        strPort = szPort;
        model = NULL;
        appOwner = NULL;
    }
    ~Arbitrator() { }

    void setPort(const char* szPort) { if(szPort) strPort = szPort; }

    const char* getPort(void) { return strPort.c_str(); }

    const char* getRule(const char* con)
    {
        if(con && (rules.find(con) != rules.end())) {
            return rules[con].c_str();
        }
        return NULL;
    }

    std::map<std::string, std::string>& getRuleMap(void) { return rules; }
    int ruleCount(void) { return rules.size(); }

    GraphicModel* getModel(void) { return model;}
    void setModel(GraphicModel* mdl) { model = mdl; };

    // modelBased is used to keep the graphic and geometric
    // information which is directly loaded from application
    // description file.
    GraphicModel& getModelBase(void) { return modelBase;}
    void setModelBase(GraphicModel& mdl) { modelBase = mdl; };

    void setOwner(Node* owner){ appOwner = owner; }
    Node* owner(void) { return appOwner; }

    bool operator==(const Arbitrator& alt) const {
        return (strPort == alt.strPort);
    }

    std::map<std::string, double>& getAlphas( const char* con) {
        return alphas[con];
    }

    double getBias(const char* con) {
        return biases[con];
    }

    void addRule(const char* con, const char* rule);
    void removeRule(const char* con);
    bool trainWeights(const char* opnd);
    bool trainWeights(void);
    bool validate(void);

protected:

private:
    std::string strPort;
    std::map<std::string, std::string> rules;
    GraphicModel* model;
    GraphicModel modelBase;
    Node* appOwner;

    std::map<std::string, std::map<std::string, double> > alphas;
    std::map<std::string, double> biases;

};


typedef std::vector<Arbitrator> ArbContainer;
typedef std::vector<Arbitrator>::iterator ArbIterator;

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_ARBITRATOR__
