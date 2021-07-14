/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/utility.h>
#include <yarp/manager/graph.h>
#include <yarp/manager/module.h>
#include <yarp/manager/application.h>
#include <yarp/manager/resource.h>

#include <cstdio>
#include <fstream>


using namespace yarp::manager;
using namespace std;


//#if defined(_MSC_VER) && (_MSC_VER == 1600)

StrStream::StrStream() = default;

StrStream::StrStream(const std::string str) {
    dummyStr = str;
}

StrStream::~StrStream() = default;

std::string StrStream::str() {
    return dummyStr;
}

StrStream& StrStream::operator<<(StrStream &oss) {
    dummyStr += oss.str();
    return *this;
}

StrStream& StrStream::operator<<(const std::string &str) {
    dummyStr += str;
    return *this;
}

StrStream& StrStream::operator<<(int n) {
    char buff[64];
    sprintf(buff, "%d", n);
    dummyStr += std::string(buff);
    return *this;
}

StrStream& StrStream::operator<<(double n) {
    char buff[64];
    sprintf(buff, "%.2f", n);
    dummyStr += std::string(buff);
    return *this;
}


StrStream& StrStream::operator = (const char* sz) {
    dummyStr = std::string(sz);
    return *this;
}

StrStream& StrStream::operator = (char* sz) {
    dummyStr = std::string(sz);
    return *this;
}

StrStream& StrStream::operator = (const std::string &str) {
    dummyStr = str;
    return *this;
}

StrStream& StrStream::operator = (StrStream &oss) {
    dummyStr = oss.str();
    return *this;
}

StrStream& StrStream::operator = (int n) {
    char buff[64];
    sprintf(buff, "%d", n);
    dummyStr = std::string(buff);
    return *this;
}

std::ostream& operator << (std::ostream &os , StrStream& sstr)
{
    std::cout<<sstr.str();
    return os;
}

//#endif


/**
 * Singleton class ErrorLogger
 */

ErrorLogger* ErrorLogger::Instance()
{
    static ErrorLogger instance;
    return &instance;
}

void ErrorLogger::addWarning(const char* szWarning) {
    if (szWarning) {
        warnings.emplace_back(szWarning);
    }
}

void ErrorLogger::addWarning(const string &str) {
    warnings.push_back(str);
}

void ErrorLogger::addWarning(OSTRINGSTREAM &stream) {
    addWarning(stream.str());
}

void ErrorLogger::addError(const char* szError) {
    if (szError) {
        errors.emplace_back(szError);
    }
}

void ErrorLogger::addError(const string &str) {
    errors.push_back(str);
}

void ErrorLogger::addError(OSTRINGSTREAM &stream) {
    addError(stream.str());
}

const char* ErrorLogger::getLastError() {
    if (errors.empty()) {
        return nullptr;
    }
    static string msg;
    msg = errors.back();
    errors.pop_back();
    return msg.c_str();
}

const char* ErrorLogger::getFormatedErrorString() {
    static string msgs;
    char* err;
    while ((err = (char*)getLastError()) != nullptr) {
        msgs += string(err) + " ";
    }
    return msgs.c_str();
}

const char* ErrorLogger::getLastWarning() {
    if (warnings.empty()) {
        return nullptr;
    }
    static string msg;
    msg = warnings.back();
    warnings.pop_back();
    return msg.c_str();
}

const char* ErrorLogger::getFormatedWarningString() {
    static string msgs;
    char* err;
    while ((err = (char*)getLastWarning()) != nullptr) {
        msgs += string(err) + " ";
    }
    return msgs.c_str();
}


void ErrorLogger::clear() {
    errors.clear(); warnings.clear();
}

int ErrorLogger::errorCount() {
    return errors.size();
}

int ErrorLogger::warningCount() {

    return warnings.size();
}


const string GRAPH_LEGEND =
"{"
"   rank=sink;"
"   style=filled;"
"   color=lightgrey;"
"    Legend [shape=none, margin=0, label=<"
"   <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" bgcolor=\"white\">"
"     <TR>"
"       <TD COLSPAN=\"2\"><B>Legend</B></TD>"
"      </TR>"
"     <TR>"
"      <TD align=\"left\">Application</TD>"
"      <TD CELLPADDING=\"4\">"
"       <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">"
"        <TR>"
"         <TD BGCOLOR=\"darkseagreen\">   </TD>"
"        </TR>"
"       </TABLE>"
"      </TD>"
"     </TR>"
"     <TR>"
"      <TD align=\"left\">Module</TD>"
"      <TD CELLPADDING=\"4\">"
"       <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">"
"        <TR>"
"         <TD BGCOLOR=\"lightslategrey\">   </TD>"
"        </TR>"
"       </TABLE>"
"      </TD>"
"     </TR>"
"     <TR>"
"      <TD align=\"left\">Res. Dependency</TD>"
"      <TD CELLPADDING=\"4\">"
"       <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">"
"        <TR>"
"         <TD BGCOLOR=\"salmon\">   </TD>"
"        </TR>"
"       </TABLE>"
"      </TD>"
"     </TR>"
"     <TR>"
"      <TD align=\"left\">Res. Provider</TD>"
"      <TD CELLPADDING=\"4\">"
"       <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">"
"        <TR>"
"         <TD BGCOLOR=\"indianred\">   </TD>"
"        </TR>"
"       </TABLE>"
"      </TD>"
"     </TR>"
"     <TR>"
"      <TD align=\"left\">Input data</TD>"
"      <TD CELLPADDING=\"4\">"
"       <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">"
"        <TR>"
"         <TD BGCOLOR=\"lightgrey\">   </TD>"
"        </TR>"
"       </TABLE>"
"      </TD>"
"     </TR>"
"     <TR>"
"      <TD align=\"left\">Output data</TD>"
"      <TD CELLPADDING=\"4\">"
"       <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">"
"        <TR>"
"         <TD BGCOLOR=\"wheat\">   </TD>"
"        </TR>"
"       </TABLE>"
"      </TD>"
"     </TR>"
"    </TABLE>"
"   >];"
"}";


/*
Carrier strToCarrier(const char* szCar)
{
    if(szCar)
    {
        if(compareString(szCar, "TCP"))
            return TCP;
        if(compareString(szCar, "UDP"))
            return UDP;
        if(compareString(szCar, "MCAST"))
            return MCAST;
        if(compareString(szCar, "SHMEM"))
            return SHMEM;
        if(compareString(szCar, "TEXT"))
            return TEXT;
    }
    return UNKNOWN;
}

const char* carrierToStr(Carrier cr)
{
    switch(cr){
        case TCP:{return("tcp");}
        case UDP:{return("udp");}
        case MCAST:{return("mcast");}
        case SHMEM:{return("shmem");}
        case TEXT:{return("text");}
        default:{return("tcp");}
     };
}
*/

OS yarp::manager::strToOS(const char* szOS)
{
    if(szOS)
    {
        if (compareString(szOS, "LINUX")) {
            return LINUX;
        }
        if (compareString(szOS, "WINDOWS")) {
            return WINDOWS;
        }
        if (compareString(szOS, "MAC")) {
            return MAC;
        }
    }
    return OTHER;
}


bool yarp::manager::compareString(const char* szFirst, const char* szSecond)
{
    if (!szFirst && !szSecond) {
        return true;
    }
    if (!szFirst || !szSecond) {
        return false;
    }

    string strFirst(szFirst);
    string strSecond(szSecond);
    transform(strFirst.begin(), strFirst.end(), strFirst.begin(),
              (int(*)(int))toupper);
    transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
              (int(*)(int))toupper);
    if (strFirst == strSecond) {
        return true;
    }
    return false;
}

void yarp::manager::trimString(string& str)
{
    string::size_type pos = str.find_last_not_of(' ');
    if(pos != string::npos)
    {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if (pos != string::npos) {
            str.erase(0, pos);
        }
    } else {
        str.erase(str.begin(), str.end());
    }
}


bool yarp::manager::exportDotGraph(Graph& graph, const char* szFileName)
{
    ofstream dot;
    dot.open(szFileName);
    if (!dot.is_open()) {
        return false;
    }

    dot<<"digraph G {"<<endl;
    dot<<"rankdir=LR;"<<endl;
    dot<<"ranksep=0.0;"<<endl;
    dot<<"nodesep=0.2;"<<endl;

    for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)
    {
        switch((*itr)->getType()) {
            case MODULE: {
                    auto* mod = (Module*)(*itr);
                    dot<<"\""<<mod->getLabel()<<"\"";
                    dot<<" [label=\""<< mod->getName()<<"\"";
                    dot<<" shape=component, color=midnightblue, fillcolor=lightslategrey, peripheries=1, style=filled, penwidth=2];"<<endl;
                    for(int i=0; i<mod->sucCount(); i++)
                    {
                        Link l = mod->getLinkAt(i);
                        auto* in = (InputData*)l.to();
                        dot<<"\""<<mod->getLabel()<<"\" -> ";
                        dot<<"\""<<in->getLabel()<<"\"";
                        if (!l.isVirtual()) {
                            dot<<" [label=\"\"];"<<endl;
                        } else {
                            dot << " [label=\"\" style=dashed];" << endl;
                        }
                    }

                    break;
                }
            case INPUTD:{
                    auto* in = (InputData*)(*itr);
                    dot<<"\""<<in->getLabel()<<"\"";
                    if(in->withPriority())
                    {
                        dot<<" [color=red, fillcolor=lightgrey, peripheries=1, style=filled";
                        dot<<" label=\""<< in->getName()<<"\\n"<<in->getPort()<<"\"];"<<endl;
                    }
                    else
                    {
                        dot<<" [color=black, fillcolor=lightgrey, peripheries=1, style=filled";
                        dot<<" label=\""<< in->getName()<<"\\n"<<in->getPort()<<"\"];"<<endl;
                    }
                    for(int i=0; i<in->sucCount(); i++)
                    {
                        Link l = in->getLinkAt(i);
                        auto* out = (OutputData*)l.to();
                        dot<<"\""<<in->getLabel()<<"\" -> ";
                        dot<<"\""<<out->getLabel()<<"\"";
                        if (!l.isVirtual()) {
                            dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
                        } else {
                            dot << " [label=\"" << l.weight() << "\" style=dashed];" << endl;
                        }
                    }

                    break;
                }
            case OUTPUTD:{
                    auto* out = (OutputData*)(*itr);
                    dot<<"\""<<out->getLabel()<<"\"";
                    dot<<" [color=black, fillcolor=wheat, peripheries=1, style=filled";
                    dot<<" label=\""<< out->getName()<<"\\n"<<out->getPort()<<"\"];"<<endl;
                    for(int i=0; i<out->sucCount(); i++)
                    {
                        Link l = out->getLinkAt(i);
                        auto* mod = (Module*)l.to();
                        dot<<"\""<<out->getLabel()<<"\" -> ";
                        dot<<"\""<<mod->getLabel()<<"\"";
                        dot<<" [label=\"\" arrowhead=none];"<<endl;
                    }

                    break;
                }

            case APPLICATION:{
                    auto* app = (Application*)(*itr);
                    dot<<"\""<<app->getLabel()<<"\"";
                    dot<<" [shape=folder, color=darkgreen, fillcolor=darkseagreen, peripheries=1, style=filled, penwidth=2";
                    dot<<" label=\""<<app->getLabel()<<"\""<<"];"<<endl;
                    for(int i=0; i<app->sucCount(); i++)
                    {
                        Link l = app->getLinkAt(i);
                        auto* mod = (Module*)l.to();
                        dot<<"\""<<app->getLabel()<<"\" -> ";
                        dot<<"\""<<mod->getLabel()<<"\"";
                        if (!l.isVirtual()) {
                            dot<<" [label=\"\"];"<<endl;
                        } else {
                            dot << " [label=\"\" style=dashed];" << endl;
                        }
                    }
                    break;
            }

            case RESOURCE:{
                    auto* res = (GenericResource*)(*itr);
                    dot<<"\""<<res->getLabel()<<"\"";
                    if (res->owner()) {
                        dot<<" [shape=rect, color=black, fillcolor=salmon, peripheries=1, style=filled ";
                    } else {
                        dot << " [shape=house, color=maroon, fillcolor=indianred, peripheries=1, style=filled, penwidth=2";
                    }
                    dot<<" label=\""<<res->getName()<<"\""<<"];"<<endl;
                    for(int i=0; i<res->sucCount(); i++)
                    {
                        Link l = res->getLinkAt(i);
                        Node* prov = l.to();
                        dot<<"\""<<res->getLabel()<<"\" -> ";
                        dot<<"\""<<prov->getLabel()<<"\"";
                        dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
                    }

                    break;
            }

            default:
                break;
        };
    }

    dot<<GRAPH_LEGEND;
    dot<<"}"<<endl;
    dot.close();
    return true;
}
