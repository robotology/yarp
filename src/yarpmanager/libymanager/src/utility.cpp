/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <fstream>
#include "utility.h"
#include "graph.h"
#include "module.h"
#include "application.h"
#include "resource.h"

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


// Global static pointer used to ensure a single instance of the class.
ErrorLogger* ErrorLogger::pInstance = NULL;  
 
ErrorLogger* ErrorLogger::Instance(void)
{
    if (!pInstance)
      pInstance = new ErrorLogger;
    return pInstance;
} 


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

OS strToOS(const char* szOS)
{
    if(szOS) 
    {
        if(compareString(szOS, "LINUX"))
            return LINUX;
        if (compareString(szOS, "WINDOWS"))
            return WINDOWS;
        if (compareString(szOS, "MAC"))
            return MAC;
    }   
    return OTHER;
}


bool compareString(const char* szFirst, const char* szSecond) 
{
    if(!szFirst && !szSecond)
        return true; 
    if( !szFirst || !szSecond)
        return false;
        
    string strFirst(szFirst);
    string strSecond(szSecond);
    transform(strFirst.begin(), strFirst.end(), strFirst.begin(), 
              (int(*)(int))toupper);
    transform(strSecond.begin(), strSecond.end(), strSecond.begin(),
              (int(*)(int))toupper);
    if(strFirst == strSecond) 
        return true; 
    return false;
}


bool exportDotGraph(Graph& graph, const char* szFileName)
{
    ofstream dot; 
    dot.open(szFileName);
    if(!dot.is_open())
        return false;
    
    dot<<"digraph G {"<<endl;
    dot<<"rankdir=LR;"<<endl;
    dot<<"ranksep=0.0;"<<endl;
    dot<<"nodesep=0.2;"<<endl;

    for(GraphIterator itr=graph.begin(); itr!=graph.end(); itr++)   
    {
        switch((*itr)->getType()) {
            case MODULE: {
                    Module* mod = (Module*)(*itr);
                    dot<<"\""<<mod->getLabel()<<"\"";
                    dot<<" [label=\""<< mod->getName()<<"\"";
                    dot<<" shape=component, color=midnightblue, fillcolor=lightslategrey, peripheries=1, style=filled, penwidth=2];"<<endl;
                    for(int i=0; i<mod->sucCount(); i++)
                    {
                        Link l = mod->getLinkAt(i);
                        InputData* in = (InputData*)l.to();
                        dot<<"\""<<mod->getLabel()<<"\" -> ";
                        dot<<"\""<<in->getLabel()<<"\"";
                        if(!l.isVirtual())
                            dot<<" [label=\"\"];"<<endl;
                        else
                            dot<<" [label=\"\" style=dashed];"<<endl;
                        
                    }
                    
                    break;
                }
            case INPUTD:{
                    InputData* in = (InputData*)(*itr);
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
                        OutputData* out = (OutputData*)l.to();
                        dot<<"\""<<in->getLabel()<<"\" -> ";
                        dot<<"\""<<out->getLabel()<<"\"";
                        if(!l.isVirtual())
                            dot<<" [label=\""<<l.weight()<<"\"];"<<endl;
                        else
                            dot<<" [label=\""<<l.weight()<<"\" style=dashed];"<<endl;
                    }
                    
                    break;
                }               
            case OUTPUTD:{
                    OutputData* out = (OutputData*)(*itr);
                    dot<<"\""<<out->getLabel()<<"\"";
                    dot<<" [color=black, fillcolor=wheat, peripheries=1, style=filled";
                    dot<<" label=\""<< out->getName()<<"\\n"<<out->getPort()<<"\"];"<<endl;
                    for(int i=0; i<out->sucCount(); i++)
                    {
                        Link l = out->getLinkAt(i);
                        Module* mod = (Module*)l.to();
                        dot<<"\""<<out->getLabel()<<"\" -> ";
                        dot<<"\""<<mod->getLabel()<<"\"";
                        dot<<" [label=\"\" arrowhead=none];"<<endl;
                    }
                    
                    break;
                }
            
            case APPLICATION:{
                    Application* app = (Application*)(*itr);
                    dot<<"\""<<app->getLabel()<<"\"";
                    dot<<" [shape=folder, color=darkgreen, fillcolor=darkseagreen, peripheries=1, style=filled, penwidth=2";
                    dot<<" label=\""<<app->getLabel()<<"\""<<"];"<<endl;                    
                    for(int i=0; i<app->sucCount(); i++)
                    {
                        Link l = app->getLinkAt(i);
                        Module* mod = (Module*)l.to();
                        dot<<"\""<<app->getLabel()<<"\" -> ";
                        dot<<"\""<<mod->getLabel()<<"\"";
                        if(!l.isVirtual())
                            dot<<" [label=\"\"];"<<endl;
                        else
                            dot<<" [label=\"\" style=dashed];"<<endl;
                    }
                    break;
            }
            
            case RESOURCE:{
                    GenericResource* res = (GenericResource*)(*itr);
                    dot<<"\""<<res->getLabel()<<"\"";
                    if(res->owner())
                        dot<<" [shape=rect, color=black, fillcolor=salmon, peripheries=1, style=filled ";
                    else
                        dot<<" [shape=house, color=maroon, fillcolor=indianred, peripheries=1, style=filled, penwidth=2";
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

