/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/binexparser.h>
#include <yarp/os/Log.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cinttypes>
#include <climits>
#include <cstddef>


#define EXPNOT     '~'
#define EXPAND     '&'
#define EXPOR      '|'
#define IMPLY      ':'

#define PRECISION(max_value) sizeof(max_value) * 8


using namespace std;
using namespace yarp::manager;


BinaryExpParser::BinaryExpParser() = default;

BinaryExpParser::~BinaryExpParser() = default;

bool BinaryExpParser::parse(string _exp)
{
    expression = _exp;
    string strexp = expression;
    if (!checkExpression(strexp)) {
        return false;
    }

    binTree.clear();
    operands.clear();
    BinaryNodePtr root = nullptr;
    parseExpression(strexp, root);
    if(root == nullptr)
    {
        ErrorLogger* logger = ErrorLogger::Instance();
        string msg = "BinaryExpParser: failed to parse the expression";
        logger->addError(msg);
        return false;
    }
    if(invalidOperands.size())
    {
        ErrorLogger* logger = ErrorLogger::Instance();
        string msg = "Invalid operands";
        for (const auto& invalidOperand : invalidOperands) {
            msg += " '" + invalidOperand + "'";
        }
        logger->addError(msg);
        return false;
    }

    // creating a truth table for inputs and outputs
    createTruthTable(operands.size());
    int n = truthTable.size();
    for(int x = 0; x < (1 << (n-1)); ++x)
    {
        auto itr = operands.begin();
        for (int y = 0; y < (n - 1); ++y) {
            (*itr++).second = (truthTable[y][x] != 0);
        }
        truthTable[n-1][x] = evalTree(root, operands);
    }
    //printTruthTable(leftOpr);
    return true;
}

bool BinaryExpParser::exportDotGraph(const char* szFileName)
{
    ofstream dot;
    dot.open(szFileName);
    if (!dot.is_open()) {
        return false;
    }

    dot<<"digraph G {"<<endl;
    for(GraphIterator itr=binTree.begin(); itr!=binTree.end(); itr++)
    {
        switch((*itr)->getType()) {
            case OPERATOR: {
                    auto node = (BinaryNodePtr)(*itr);
                    dot<<"\""<<node->getLabel()<<"\"";
                    dot<<" [label=\""<< node->getName()<<"\"";
                    dot<<" shape=circle, fillcolor=lightslategrey, style=filled];"<<endl;
                    for(int i=0; i<node->sucCount(); i++)
                    {
                        Link l = node->getLinkAt(i);
                        auto to = (BinaryNodePtr)l.to();
                        dot<<"\""<<node->getLabel()<<"\" -> ";
                        dot<<"\""<<to->getLabel()<<"\"";
                        dot<<" [label=\"\"];"<<endl;
                    }
                    break;
                }
             case OPERAND: {
                    auto node = (BinaryNodePtr)(*itr);
                    dot<<"\""<<node->getLabel()<<"\"";
                    dot<<" [label=\""<< node->getName()<<"\"";
                    dot<<" shape=square];"<<endl;
                    break;
                }
            default:
                break;
        };
    }
    dot<<"}"<<endl;
    dot.close();
    return true;
}


bool BinaryExpParser::evalTree(BinaryNodePtr node, std::map<std::string, bool>& opnd)
{
    bool result = false;
    if(node->isLeaf())
    {
        node->setValue(opnd[node->getName()]);
        result = node->getValue();
    }
    else
    {
        if(strcmp(node->getName(), "~") == 0)
        {
            auto left = (BinaryNodePtr)node->getLinkAt(0).to();
            result = !evalTree(left, opnd);
            node->setValue(result);
        }
        else if(strcmp(node->getName(), "&") == 0)
        {
            auto left = (BinaryNodePtr)node->getLinkAt(0).to();
            auto right = (BinaryNodePtr)node->getLinkAt(1).to();
            result = evalTree(left, opnd) && evalTree(right, opnd);
            node->setValue(result);
        }
        else if(strcmp(node->getName(), "|") == 0)
        {
            auto left = (BinaryNodePtr)node->getLinkAt(0).to();
            auto right = (BinaryNodePtr)node->getLinkAt(1).to();
            result = evalTree(left, opnd) || evalTree(right, opnd);
            node->setValue(result);
        }
    }
    return result;
}

bool isParentheses (char c)
{
    return ((c=='(') || (c==')'));
}

bool BinaryExpParser::checkExpression(std::string& strexp)
{
    ErrorLogger* logger = ErrorLogger::Instance();

    if(std::count(strexp.begin(), strexp.end(), '(') !=
        std::count(strexp.begin(), strexp.end(), ')'))
    {
        logger->addError("Incorrect expression format! (parentheses do not match)");
        return false;
    }
    if(std::count(strexp.begin(), strexp.end(), IMPLY) != 1 )
    {
        logger->addError("Incorrect expression format! (no implication ':' found)");
        return false;
    }

    // erassing all the sapces
    strexp.erase(std::remove_if(strexp.begin(), strexp.end(), ::isspace), strexp.end());
    if(!strexp.size())
    {
       logger->addError("Empty expression!");
       return false;
    }

    // making a copy of strexp and checking more
    string  dummy = strexp;
    // removing all pranteses
    dummy.erase(std::remove_if(dummy.begin(), dummy.end(), isParentheses), dummy.end());
    leftOpr = dummy.substr(0, dummy.find(IMPLY));
    if(!leftOpr.size())
    {
        logger->addError("Missing operand before ':'");
        return false;
    }
    if(dummy.find(IMPLY) == (dummy.size()-1))
    {
        logger->addError("Missing operands after ':'");
        return false;
    }

    dummy.erase(0, dummy.find(IMPLY)+1);
    if(dummy.find(leftOpr) != string::npos)
    {
        std::string msg;
        msg = "recursive assignment of operand '" + leftOpr + "'";
        logger->addError(msg.c_str());
        return false;
    }

    // checking '~'
    size_t n = dummy.find(EXPNOT);
    while(n != string::npos)
    {
        OSTRINGSTREAM msg;
        bool bError = ((n+1) == dummy.length()); // empty operand after ~
        if((n+1) < dummy.length())
        {
            bError |= (dummy[n+1] == EXPAND);        // operator & after ~
            bError |= (dummy[n+1] == EXPOR);         // operand | after ~
        }
        if (n != 0) {
            bError |= (dummy[n - 1] != EXPAND) && (dummy[n - 1] != EXPOR); // an operand before ~
        }
        if(bError)
        {
            msg<<"Incorrect expression format of '~' at "<<(int)n;
            logger->addError(msg.str().c_str());
            return false;
        }
        n = dummy.find(EXPNOT, n+1);
    }

    // checking '| &'
    n = dummy.find_first_of("&|");
    while(n != string::npos)
    {
        OSTRINGSTREAM msg;
        bool bError = ((n+1) == dummy.length());    // empty operand after & or |
        if((n+1) < dummy.length())
        {
            bError |= (dummy[n+1] == EXPAND);        // operator & after & or |
            bError |= (dummy[n+1] == EXPOR);         // operand | after & or |
        }
        bError |= (n == 0);                      // empty operand before & or |
        if(n != 0)
        {
            bError |= (dummy[n-1] == EXPAND);        // operator & before & or |
            bError |= (dummy[n-1] == EXPOR);         // operand | before & or |
            bError |= (dummy[n-1] == EXPOR);         // operand ~ before & or |
        }
        if(bError)
        {
            msg<<"Incorrect expression format of '&' or '|' at "<<(int)n;
            logger->addError(msg.str().c_str());
            return false;
        }
        n = dummy.find_first_of("&|", n+1);
    }

    // at the end
    strexp.erase(0, strexp.find(IMPLY)+1);
    return true;
}

void BinaryExpParser::parseExpression(std::string &strexp, BinaryNodePtr& node)
{
    string op;
    BinaryNodePtr rightNode = nullptr;
    parseNot(strexp, node);
    while(!strexp.empty() &&
          ((*strexp.begin() == EXPAND) ||
          (*strexp.begin() == EXPOR)))
    {
        op = *strexp.begin();
        strexp.erase(strexp.begin());
        parseNot(strexp, rightNode);
        BinaryNode tmpNode(op.c_str(), node, rightNode);
        node = (BinaryNodePtr) binTree.addNode(&tmpNode);
    }
}


void BinaryExpParser::parseNot(std::string &strexp, BinaryNodePtr& node) {
    string op;
    BinaryNodePtr rightNode;
    if (*strexp.begin() != EXPNOT) {
        parseFactor(strexp, node);
    }
    while(!strexp.empty() &&
          (*strexp.begin() == EXPNOT))
    {
        op = *strexp.begin();
        strexp.erase(strexp.begin());
        parseFactor(strexp, rightNode);
        BinaryNode tmpNode(op.c_str(), rightNode, nullptr);
        node = (BinaryNodePtr) binTree.addNode(&tmpNode);
    }
}

void BinaryExpParser::parseFactor(std::string &strexp, BinaryNodePtr& node) {
    if(!strexp.empty() && (*strexp.begin() != '('))
    {
        string op = popNextOperand(strexp);
        BinaryNode tmpNode(op.c_str());
        node = (BinaryNodePtr) binTree.addNode(&tmpNode);
        operands[op] = false;
        if(validOperands.size())
        {
            if (std::find(validOperands.begin(),
                          validOperands.end(),
                          op)
                == validOperands.end()) {
                invalidOperands.push_back(op);
            }
        }
    }
    else
    {
        strexp.erase(strexp.begin());  //skip '('
        parseExpression(strexp, node);
        strexp.erase(strexp.begin()); //skip ')'
    }
}


std::string BinaryExpParser::getNextOperand(std::string &strexp) {
        string token;
        std::string::iterator it;
        for (it = strexp.begin(); it != strexp.end(); ++it) {
            if ((*it != EXPAND) && (*it != EXPOR) && (*it != EXPNOT) && (*it != ')') && (*it != '(')) {
                token.push_back(*it);
            } else {
                break;
            }
        }
        return token;
}

std::string BinaryExpParser::popNextOperand(std::string &strexp) {
        string token;
        std::string::iterator it;
        for (it = strexp.begin(); it != strexp.end(); ++it) {
            if ((*it != EXPAND) && (*it != EXPOR) && (*it != EXPNOT) && (*it != ')') && (*it != '(')) {
                token.push_back(*it);
            } else {
                strexp.erase(strexp.begin(), it);
                break;
            }
        }
        return token;
}

void BinaryExpParser::createTruthTable(const int n)
{
    yAssert((n-1) > 0);
    yAssert((unsigned)(n-1) < PRECISION(INT_MAX));
    yAssert(1 <= (INT_MAX >> (n-1)));

    truthTable.clear();
    // n input + one output
    truthTable.resize(n+1);
    for (int i = 0; i < n + 1; i++) {
        truthTable[i].resize(1 << n);
    }
    int num_to_fill = 1 << (n - 1);
    for(int col = 0; col < n; ++col, num_to_fill >>= 1)
    {
        for(int row = num_to_fill; row < (1 << n); row += (num_to_fill * 2))
        {
            std::fill_n(&truthTable[col][row], num_to_fill, 1);
        }
    }
}

void BinaryExpParser::printTruthTable(std::string lopr)
{
    int n = truthTable.size();

    yAssert((n-1) > 0);
    yAssert((unsigned)(n-1) < PRECISION(INT_MAX));
    yAssert(1 <= (INT_MAX >> (n-1)));

    map<string, bool>::iterator itr;
    for (itr = operands.begin(); itr != operands.end(); itr++) {
        cout << (*itr).first << "\t";
    }
    cout<<lopr<<endl;
    for (int i = 0; i < (int)operands.size() * 8 + 1; i++) {
        cout << "-";
    }
    cout<<endl;

    for(int x = 0; x < (1<<(n-1)); ++x)
    {
        for (int y = 0; y < n; ++y) {
            std::cout << truthTable[y][x] << "\t";
        }
        std::cout<<std::endl;
    }
}


bool LinkTrainer::train(const std::vector<std::vector<int> >&  truthTable)
{
    // resetting weights
    int n = truthTable.size();
    errors.clear();
    alphas.clear();
    alphas.resize(n-1);
    std::fill(alphas.begin(), alphas.end(), 0.0);
    bias = 0.0;
    double sumerr = 1.0;
    for(int i=0; (i<maxIteration) && (sumerr !=0); i++)
    {
        sumerr = 0;
        // number of training set : (1 << (n-1)
        for(int x = 0; x < (1 << (n-1)); ++x)
        {
            // computing output of one set
            double P = truthTable[n-1][x];
            double out = bias;
            for (int y = 0; y < n - 1; ++y) {
                out = out + ((double)truthTable[y][x] * alphas[y]);
            }
            out = (out>0) ? 1 : 0;
            double err = P - out;
            sumerr += fabs(err);
            //cout<<P<<"\t"<<out<<"\terr:"<<err<<endl;
            bias = bias + trainRate * err;
            for (int y = 0; y < (n - 1); ++y) {
                alphas[y] = alphas[y] + (trainRate * err * (double)truthTable[y][x]);
            }
        }
        errors.push_back(sumerr);
    }
    return ((int)errors.size() < maxIteration);
}
