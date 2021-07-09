/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>

#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

using namespace yarp::companion::impl;
using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp;

namespace {

const std::string schedHeader =
       "| Policy label | Policy (dec) | Priority min (dec) | Priority max (dec) |\n"
       "|--------------|--------------|--------------------|--------------------|";

const std::map<int,std::string> schedTable = {
    {0, schedHeader},
    {1,"| SCHED_OTHER  | 0            | 0                  | 0                  |"},
    {2,"| SCHED_FIFO   | 1            | 1                  | 99                 |"},
    {3,"| SCHED_RR     | 2            | 1                  | 99                 |"}};

const std::map<std::string,int> schedBrowser = {{"SCHED_OTHER",1},{"0",1},
                                                {"SCHED_FIFO",2},{"1",2},
                                                {"SCHED_RR",3},{"2",3}};

const std::map<int,std::string> policyLabels = {{-1,"INVALID"},{0,"SCHED_OTHER"},{1,"SCHED_FIFO"},{2,"SCHED_RR"}};

const std::map<int,std::vector<int>> priorityLimitsMap = {{0,{0,0}},{1,{1,99}},{2,{1,99}}};

} // namespace


int Companion::cmdPrioritySched(int argc, char *argv[])
{
    if(argc==0){
        yCError(COMPANION, "Not enough parameters/options. If not sure on how to use the command, please type \"yarp priority-sched --help\" for instructions");
        return 1;
    }

    std::string mode;
    mode = std::string(argv[0]);
    argc--;
    argv++;
    if(mode=="--help"){
        yCInfo(COMPANION) << R"===(## yarp priority-sched: a companion command for getting/setting thread priority and policy priority for a connection
# Syntax:

#    $ yarp priority-sched <mode> [options...]

# <mode> argument can be:
#    - describe    It shows a table with useful info about the thread policies values and their ranges of priorities
#      + syntax:   $ yarp priority-sched describe [value]
#        . value   It can be an integer (policy int value), a policy label (e.g. "SCHED_RR")
#                  If omitted, the whole table will be shown, etherwise, only the line corresponding to the specified value
##                  will be printed
#    - get             It returns the thread policy and/or priority values of the connection
#      + syntax:       $ yarp priority-sched get <src-port> <des-port> [port] [param]
#        . src-port    It's the name of the source port of the selected connection (from now on "/w")
#        . des-port    It's the name of the destination port of the selected connection (from now on "/r")
#        . port        The port you want to get data from. The values can be "--src" for /w or
#                      "--des" for /r. If omitted, data from both the ports will be returned
#        . param       The wanted param you want to get. It can be "--policy" if you want to get the thread
#                      policy as an integer. Than, it can be "--policy-label" if you want the same value but
#                      represented as a label. The last possible value is "--priority" if you want to get info
#                      about the thread priority. If "param" is omitted, the command will return both
#                      parameters a integers
#    - set             It sets the thread policy or priority value for a specific port.
#      + syntax        $ yarp priority-sched get <src-port> <des-port> <port> <value>
#        . src-port    As for "get", it is the name of /w
#        . des-port    As for "get", it is the name of /r
#        . port        The port to which send the new policy/priority value. The values can be "--src" for /w or "--des" for /r
#        . value       The value to set. t can then be "LABEL:value" where "LABEL" can be "PRT" or "POL".
#                      In the first case, the value can be a policy label (e.g. "SCHED_FIFO") or a decimal integer value between 0 and 2
#                      If the currently set priority is outside the newly set policy, the command will warn you and then set
#                      as priority the minimum allowed value for the new policy. In the second case, the value can only be
#                      the priority integer value. If this value is outside the allowed priority range for the currently set
#                      policy, the command will not set it and return an error message.)===";
    }
    else if(mode=="describe"){
        if(argc==0){
            yCInfo(COMPANION, "%s",printTable(schedTable,0).c_str());
        }
        else{
            std::string toDescribe = std::string(argv[0]);
            if(schedBrowser.count(toDescribe)!=0){
                yCInfo(COMPANION, "%s",printTable(schedTable,schedBrowser.at(toDescribe)).c_str());
            }
        }
    }
    else if(mode=="get" || mode=="set"){
        if(argc<2){
            yCError(COMPANION, "Get/set mode must have two ports involved in the connection as parameters (argc>=2)");
            return 1;
        }
        std::string src, des;
        src = std::string(argv[0]);
        des = std::string(argv[1]);
        if(!Network::exists(src) || !Network::exists(des)){
            yCError(COMPANION, "%s not found. Please check again", (!Network::exists(src) ? src : (!Network::exists(src) == !Network::exists(des) ? "Neither "+src+" nor "+des : des)).c_str());
            return 1;
        }
        QosStyle srcStyle, desStyle;
        Network::getConnectionQos(src,des,srcStyle,desStyle);
        if(mode=="get"){
            std::vector<std::string> labels = {"policy","policy-label","priority"};
            int srcPol = srcStyle.getThreadPolicy();
            int srcPrt = srcStyle.getThreadPriority();
            int desPol = desStyle.getThreadPolicy();
            int desPrt = desStyle.getThreadPriority();
            if(argc==2){
                yCInfo(COMPANION, "(src (sched ((policy %d) (priority %d))))(des (sched ((policy %d) (priority %d))))",srcPol,srcPrt,desPol,desPrt);
            }
            else if(argc==3){
                std::string argv2 = std::string(argv[2]).substr(2);
                std::vector<std::string>::iterator it = std::find(labels.begin(), labels.end(), argv2);
                if(it!=labels.end()){
                    int index = std::distance(labels.begin(), it);
                    switch(index){
                    case 0: {
                        yCInfo(COMPANION, "(src (sched ((policy %d))))(des (sched ((policy %d))))",srcPol,desPol);
                        break;
                    }
                    case 1: {
                        yCInfo(COMPANION, "(src (sched ((policy %s))))(des (sched ((policy %s))))",policyLabels.at(srcPol).c_str(),policyLabels.at(desPol).c_str());
                        break;
                    }
                    case 2: {
                        yCInfo(COMPANION, "(src (sched ((priority %d))))(des (sched ((priority %d))))",srcPrt,desPrt);
                        break;
                    }
                    }
                }
                else{
                    if(argv2=="src"){
                        yCInfo(COMPANION, "(src (sched ((policy %d) (priority %d))))",srcPol,srcPrt);
                    }
                    else if(argv2=="des"){
                        yCInfo(COMPANION, "(des (sched ((policy %d) (priority %d))))",desPol,desPrt);
                    }
                    else{
                        yCError(COMPANION, "Wrong value. Do not know what %s is",argv[2]);
                        return 1;
                    }
                }
            }
            else if(argc>=4){
                bool foundPort = false;
                bool foundParam = false;
                std::string port, format;
                std::string warningString = "";
                std::string argv2 = std::string(argv[2]).substr(2);
                std::string argv3 = std::string(argv[3]).substr(2);
                std::vector<std::string>::iterator it2 = std::find(labels.begin(), labels.end(), argv2);
                std::vector<std::string>::iterator it3 = std::find(labels.begin(), labels.end(), argv3);
                int index;
                if(it2 == labels.end() && it3 != labels.end()){
                    format = argv3;
                    index = std::distance(labels.begin(), it3);
                    foundParam = true;
                    if(argv2 == "src" || argv2 == "des"){
                        port = argv2;
                        foundPort = true;
                    }
                }
                else if(it2 != labels.end() && it3 == labels.end()){
                    format = argv2;
                    index = std::distance(labels.begin(), it2);
                    foundParam = true;
                    if(argv3 == "src" || argv3 == "des"){
                        port = argv3;
                        foundPort = true;
                    }
                }
                else {
                    foundPort = (argv3 == "src" || argv3 == "des") || (argv2 == "src" || argv2 == "des");
                }

                if(foundPort && foundParam){
                    std::string param;
                    switch(index){
                    case 0:{
                        param = std::to_string(port == "src" ? srcStyle.getThreadPolicy() : desStyle.getThreadPolicy());
                        break;
                    }
                    case 1: {
                        param = port == "src" ? policyLabels.at(srcStyle.getThreadPolicy()) : policyLabels.at(desStyle.getThreadPolicy());
                        break;
                    }
                    case 2: {
                        param = std::to_string(port == "src" ? srcStyle.getThreadPriority() : desStyle.getThreadPriority());
                        break;
                    }
                    }
                    std::ostringstream streamOut;
                    streamOut<<"("<<port<<" (sched (("<<format<<" "<<param<<"))))";
                    yCInfo(COMPANION, "%s",streamOut.str().c_str());
                }
                else{
                    yCError(COMPANION, "%s",foundPort ? "Port found but wrong parameter" : "Parameter ok, but wrong port selected");
                    return 1;
                }
            }
        }
        else{
            if(argc<4){
                yCError(COMPANION, "Set mode must have the port to apply the new parameter to and the value of the packet priority as parameters (argc>=4)");
                return 1;
            }
            std::string argv2 = std::string(argv[2]);
            std::string argv3 = std::string(argv[3]);
            std::string port = argv2 == "--src" || argv2 == "--des" ? argv2 : (argv3 == "--src" || argv3 == "--des" ? argv3 : "none");
            if(port == "none"){
                yCError(COMPANION, "You haven\'t specified a port to which to apply the changes");
                return 1;
            }
            QosStyle& toSet = port == "--src" ? srcStyle : desStyle;
            QosStyle forTest;
            std::string value = argv2 != "--src" && argv2 != "--des" ? argv2 : (argv3 != "--src" && argv3 != "--des" ? argv3 : "none");
            if(value == "none"){
                yCError(COMPANION, "You, somehow, specified two ports instead of a port and a value. Please try again");
                return 1;
            }
            std::vector<std::string> labels = {"POL:","PRT:"};
            std::string label = "";
            size_t pos = std::string::npos;
            for(auto l: labels){
                pos = value.find(l);
                if(pos!=std::string::npos){
                    label = l;
                    break;
                }
            }
            if(label==""){
                yCError(COMPANION, "The label used for the data has not been recognised. Try again (type \"yarp priority-sched --help\" for info)");
                return 1;
            }
            else{
                std::string number = value.erase(pos,label.length());
                try {
                    int tempVal = stoi(number);
                    if(label=="POL:"){
                        if(tempVal<0 || tempVal>2){
                            yCError(COMPANION, "Invalid policy value. Policy must be a value between 0 and 2");
                            return 1;
                        }
                        if(toSet.getThreadPriority()<priorityLimitsMap.at(tempVal)[0] || toSet.getThreadPriority()>priorityLimitsMap.at(tempVal)[1]){
                            yCInfo(COMPANION, "The currently set priority is not compatible with selected policy. Therefore it will be set to %d",priorityLimitsMap.at(tempVal)[0]);
                            toSet.setThreadPriority(priorityLimitsMap.at(tempVal)[0]);
                        }
                        toSet.setThreadPolicy(tempVal);
                    }
                    else{
                        int policy = toSet.getThreadPolicy();
                        if(policy == -1){
                            yCError(COMPANION, "The policy for this port is set to an invalid value. Change the policy before setting the priority value");
                            return -1;
                        }
                        if(tempVal<priorityLimitsMap.at(policy)[0] || tempVal>priorityLimitsMap.at(policy)[1]){
                            yCError(COMPANION, "Invalid priority value. Priority must be a value between %d and %d",priorityLimitsMap.at(policy)[0],priorityLimitsMap.at(policy)[1]);
                            return 1;
                        }
                        toSet.setThreadPriority(tempVal);
                    }
                }
                catch (std::invalid_argument const &e)
                {
                    if(label == "POL:"){
                        int equivalent = -1;
                        for (std::map<int,std::string>::const_iterator it=policyLabels.begin(); it!=policyLabels.end(); ++it)
                        {
                           if(it->second == number){
                               equivalent = it->first;
                               break;
                           }
                        }
                        if(equivalent != -1){
                            if(toSet.getThreadPriority()<priorityLimitsMap.at(equivalent)[0] || toSet.getThreadPriority()>priorityLimitsMap.at(equivalent)[1]){
                                yCInfo(COMPANION, "The currently set priority is not compatible with selected policy. Therefore it will be set to %d",priorityLimitsMap.at(equivalent)[0]);
                                toSet.setThreadPriority(priorityLimitsMap.at(equivalent)[0]);
                            }
                            toSet.setThreadPolicy(equivalent);
                        }
                        else{
                            yCError(COMPANION, "You provided an invalid value (%s). Please try again",number.c_str());
                            return 1;
                        }
                    }
                    else{
                        yCError(COMPANION, "Priority can only be a numerical value. Please try again");
                        return 1;
                    }
                }
                catch (std::out_of_range const &e)
                {
                    yCError(COMPANION,  "Integer overflow: std::out_of_range thrown");
                    return 1;
                }
            }
            Network::setConnectionQos(src,des,srcStyle,desStyle);
        }
    }
    else{
        yCError(COMPANION, "Unspupported mode (type \"yarp priority-sched --help\" to get help)");
    }
    return 0;
}
