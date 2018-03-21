/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MANAGER_BINEXPARSER
#define YARP_MANAGER_BINEXPARSER

#include <string>
#include <cstdio>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/graph.h>
#include <yarp/conf/api.h>

namespace yarp {
namespace manager {


typedef enum __BinNodeType {
    OPERATOR,
    OPERAND
} BinNodeType;


static int node_id = 0;

class BinaryNode : public Node
{

public:
    BinaryNode(const char* opd) : Node((NodeType)OPERAND) {
        char str[128];
        sprintf(str, "%s%d", opd, node_id++);
        setLabel(str);
        strName = opd;
        value = false;
    }

    BinaryNode(const char* opt,
               BinaryNode* left,
               BinaryNode* right ) : Node((NodeType)OPERATOR) {

        char str[128];
        sprintf(str, "%s%d", opt, node_id++);
        setLabel(str);
        strName = opt;
        value = false;
        if(left)
            addSuc(left, 0);
        if(right)
            addSuc(right, 0);
    }

    virtual ~BinaryNode() { }

    virtual Node* clone(void) override {
        BinaryNode* binode = new BinaryNode(*this);
        return binode;
    }

    BinaryNode* leftOf() {
        if(sucCount()<1)
            return NULL;
        return (BinaryNode*) getLinkAt(0).to();
    }

    BinaryNode* rightOf() {
        if(sucCount()<2)
            return NULL;
        return (BinaryNode*) getLinkAt(1).to();
    }

    bool getValue(void) { return value; }
    void setValue(bool val) { value = val; }

    const char* getName(void) {return strName.c_str(); }

protected:

private:
    bool value;
    std::string strName;

};

typedef BinaryNode* BinaryNodePtr;

class BinaryExpParser
{

public:
    BinaryExpParser();
    virtual ~BinaryExpParser();

    bool parse(std::string _exp);
    bool exportDotGraph(const char* szFileName);
    void addRestrictedOperand(const char* opnd) {
        if(opnd)
            validOperands.push_back(opnd);
    }

    const std::map<std::string, bool> &getOperands(void) {
        return operands;
    }
    const std::vector<std::vector<int> > &getTruthTable(void) {
        return truthTable;
    }

private:

    bool evalTree(BinaryNodePtr node, std::map<std::string, bool>& opnd);

    bool checkExpression(std::string& strexp);
    void parseExpression(std::string &strexp, BinaryNodePtr& node);
    void parseNot(std::string &strexp, BinaryNodePtr& node);
    void parseFactor(std::string &strexp, BinaryNodePtr& node);
    std::string getNextOperand(std::string &strexp);
    std::string popNextOperand(std::string &strexp);
    void createTruthTable(const int n);
    void printTruthTable(std::string lopr);
    //bool train(int max_itr=1000, double train_rate=1.0);

private:
    std::string expression;
    std::string leftOpr;
    Graph binTree;
    // mapping operands to their real value
    std::map<std::string, bool> operands;
    std::vector<std::string> validOperands;
    std::vector<std::string> invalidOperands;
    std::vector<std::vector<int> > truthTable;
    //std::vector<double> alphas;
    //std::vector<double> errors;
    //double bias;
};

class LinkTrainer
{
public:
    LinkTrainer(int max_itr=1000, double train_rate=1.0) :
        maxIteration(max_itr),
        trainRate(train_rate),
        bias(0.0)
    {}

    virtual ~LinkTrainer() {}

    bool train(const std::vector<std::vector<int> >  &truthTable);

    const std::vector<double> &getAlphas(void) { return alphas; }
    const std::vector<double> &getErrors(void) { return errors; }
    double getBias(void) {return bias; }

private:
    int maxIteration;
    double trainRate;
    std::vector<double> alphas;
    std::vector<double> errors;
    double bias;

};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_BINEXPARSER____
