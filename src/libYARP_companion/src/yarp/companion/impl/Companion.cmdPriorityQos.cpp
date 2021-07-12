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

const std::map<QosStyle::PacketPriorityDSCP,std::string> dscpClassesMap = {{QosStyle::DSCP_CS0,"CS0"},{QosStyle::DSCP_CS1,"CS1"},{QosStyle::DSCP_CS2,"CS2"},
                                                                           {QosStyle::DSCP_CS3,"CS3"},{QosStyle::DSCP_CS4,"CS4"},{QosStyle::DSCP_CS5,"CS5"},
                                                                           {QosStyle::DSCP_CS6,"CS6"},{QosStyle::DSCP_AF11,"AF11"},{QosStyle::DSCP_AF12,"AF12"},
                                                                           {QosStyle::DSCP_AF13,"AF13"},{QosStyle::DSCP_AF21,"AF21"},{QosStyle::DSCP_AF22,"AF22"},
                                                                           {QosStyle::DSCP_AF23,"AF23"},{QosStyle::DSCP_AF31,"AF31"},{QosStyle::DSCP_AF32,"AF32"},
                                                                           {QosStyle::DSCP_AF33,"AF33"},{QosStyle::DSCP_AF41,"AF41"},{QosStyle::DSCP_AF42,"AF42"},
                                                                           {QosStyle::DSCP_AF43,"AF43"},{QosStyle::DSCP_EF,"EF"},{QosStyle::DSCP_VA,"VA"},
                                                                           {QosStyle::DSCP_Invalid,"Invalid"},{QosStyle::DSCP_Undefined,"Undefined"}};
const std::map<QosStyle::PacketPriorityLevel,std::string> levelClassesMap = {{QosStyle::PacketPriorityLow,"LOW"},{QosStyle::PacketPriorityNormal,"NORMAL"},
                                                                             {QosStyle::PacketPriorityHigh,"HIGH"},{QosStyle::PacketPriorityCritical,"CRIT"},
                                                                             {QosStyle::PacketPriorityInvalid,"Invalid"},{QosStyle::PacketPriorityUndefined,"Undefined"}};

const std::map<std::string,int> dscpBrowser = {{"CS0",1},{"DSCP:0",1},{"NORM",1},
                                               {"CS1",2},{"DSCP:8",2},{"32",2},
                                               {"CS2",3},{"DSCP:16",3},{"64",3},
                                               {"CS3",4},{"DSCP:24",4},{"96",4},
                                               {"CS4",5},{"DSCP:32",5},{"128",5},
                                               {"CS5",6},{"DSCP:40",6},{"160",6},
                                               {"CS6",7},{"DSCP:48",7},{"192",7},
                                               {"CS7",8},{"DSCP:56",8},{"224",8},
                                               {"AF11",9},{"DSCP:10",9},{"40",9},{"LOW",9},{"LEVEL:10",9},
                                               {"AF12",10},{"DSCP:12",10},{"48",10},
                                               {"AF13",11},{"DSCP:14",11},{"56",11},
                                               {"AF21",12},{"DSCP:18",12},{"72",12},
                                               {"AF22",13},{"DSCP:20",13},{"80",13},
                                               {"AF23",14},{"DSCP:22",14},{"88",14},
                                               {"AF31",15},{"DSCP:26",15},{"104",15},
                                               {"AF32",16},{"DSCP:28",16},{"112",16},
                                               {"AF33",17},{"DSCP:30",17},{"120",17},
                                               {"AF41",18},{"DSCP:34",18},{"136",18},
                                               {"AF42",19},{"DSCP:36",19},{"144",19},{"HIGH",19},{"LEVEL:36",19},
                                               {"AF43",20},{"DSCP:38",20},{"152",20},
                                               {"VA",21},{"DSCP:44",21},{"176",21},{"CRIT",21},{"LEVEL:44",21},
                                               {"EF",22},{"DSCP:46",22},{"184",22}};

const std::string dscpHeader =
         "| DSCP  | DSCP   | DSCP  | DSCP  | TOS   | ToS   | ToS      | ToS   | ToS   | ToS   | ToS       | ToS         | ToS                 | YARP  |\n"
         "| class | (bin)  | (hex) | (dec) | (dec) | (hex) | (bin)    | Prec. | Prec. | Delay | Throghput | Reliability | Str.                | Level |\n"
         "|       |        |       |       |       |       |          | (bin) | (dec) | Flag  | Flag      | Flag        | Format              |       |\n"
         "|-------|--------|-------|-------|-------|-------|----------|-------|-------|-------|-----------|-------------|---------------------|-------|";
const  std::map<int,std::string> dscpTable = {
    {0, dscpHeader},
    {1,  "| CS0   | 000000 | 0×00  | 0     | 0     | 0×00  | 00000000 | 000   | 0     | 0     | 0         | 0           | -                   | NORM  |"},
    {2,  "| CS1   | 001000 | 0×08  | 8     | 32    | 0×20  | 00100000 | 001   | 1     | 0     | 0         | 0           | Priority            | -     |"},
    {3,  "| CS2   | 010000 | 0×10  | 16    | 64    | 0×40  | 01000000 | 010   | 2     | 0     | 0         | 0           | Immediate           | -     |"},
    {4,  "| CS3   | 011000 | 0×18  | 24    | 96    | 0×60  | 01100000 | 011   | 3     | 0     | 0         | 0           | Flash               | -     |"},
    {5,  "| CS4   | 100000 | 0×20  | 32    | 128   | 0×80  | 10000000 | 100   | 4     | 0     | 0         | 0           | FlashOverride       | -     |"},
    {6,  "| CS5   | 101000 | 0×28  | 40    | 160   | 0×A0  | 10100000 | 101   | 5     | 0     | 0         | 0           | Critical            | -     |"},
    {7,  "| CS6   | 110000 | 0×30  | 48    | 192   | 0×C0  | 11000000 | 110   | 6     | 0     | 0         | 0           | Internetworkcontrol | -     |"},
    {8,  "| CS7   | 111000 | 0×38  | 56    | 224   | 0×E0  | 11100000 | 111   | 7     | 0     | 0         | 0           | Networkcontrol      | -     |"},
    {9,  "| AF11  | 001010 | 0×0A  | 10    | 40    | 0×28  | 00101000 | 001   | 1     | 0     | 1         | 0           | Priority            | LOW   |"},
    {10, "| AF12  | 001100 | 0×0C  | 12    | 48    | 0×30  | 00110000 | 001   | 1     | 1     | 0         | 0           | Priority            | -     |"},
    {11, "| AF13  | 001110 | 0×0E  | 14    | 56    | 0×38  | 00111000 | 001   | 1     | 1     | 1         | 0           | Priority            | -     |"},
    {12, "| AF21  | 010010 | 0×12  | 18    | 72    | 0×48  | 01001000 | 010   | 2     | 0     | 1         | 0           | Immediate           | -     |"},
    {13, "| AF22  | 010100 | 0×14  | 20    | 80    | 0×50  | 01010000 | 010   | 2     | 1     | 0         | 0           | Immediate           | -     |"},
    {14, "| AF23  | 010110 | 0×16  | 22    | 88    | 0×58  | 01011000 | 010   | 2     | 1     | 1         | 0           | Immediate           | -     |"},
    {15, "| AF31  | 011010 | 0×1A  | 26    | 104   | 0×68  | 01101000 | 011   | 3     | 0     | 1         | 0           | Flash               | -     |"},
    {16, "| AF32  | 011100 | 0×1C  | 28    | 112   | 0×70  | 01110000 | 011   | 3     | 1     | 0         | 0           | Flash               | -     |"},
    {17, "| AF33  | 011110 | 0×1E  | 30    | 120   | 0×78  | 01111000 | 011   | 3     | 1     | 1         | 0           | Flash               | -     |"},
    {18, "| AF41  | 100010 | 0×22  | 34    | 136   | 0×88  | 10001000 | 100   | 4     | 0     | 1         | 0           | FlashOverride       | -     |"},
    {19, "| AF42  | 100100 | 0×24  | 36    | 144   | 0×90  | 10010000 | 100   | 4     | 1     | 0         | 0           | FlashOverride       | HIGH  |"},
    {20, "| AF43  | 100110 | 0×26  | 38    | 152   | 0×98  | 10011000 | 100   | 4     | 1     | 1         | 0           | FlashOverride       | -     |"},
    {21, "| VA    | 101100 | 0×2C  | 44    | 176   | 0×B0  | 10110000 | 101   | 5     | 1     | 0         | 0           | Critical            | CRIT  |"},
    {22, "| EF    | 101110 | 0×2E  | 46    | 184   | 0×B8  | 10111000 | 101   | 5     | 1     | 1         | 0           | Critical            | -     |"}};

} // namespace


int Companion::cmdPriorityQos(int argc, char *argv[])
{
    if(argc==0){
        yCError(COMPANION, "Not enough parameters/options. If not sure on how to use the command, please type \"yarp priority-qos --help\" for instructions");
        return 1;
    }
    std::string mode;
    mode = std::string(argv[0]);
    argc--;
    argv++;
    if(mode=="describe"){
        if(argc==0){
            yCInfo(COMPANION) << printTable(dscpTable, 0);
        }
        else{
            std::string toDescribe = std::string(argv[0]);
            if(dscpBrowser.count(toDescribe)!=0){
                yCInfo(COMPANION, "%s",printTable(dscpTable,dscpBrowser.at(toDescribe)).c_str());
            }
        }
    } // 22 spaces
    else if (mode=="--help"){
        yCInfo(COMPANION) << R"===(## yarp priority-qos: a companion command for getting/setting packet priority for a connection
# Syntax:

#    $ yarp priority-qos <mode> [options...]

# <mode> argument can be:
#    - describe    It shows a table with useful info about the packet priorities values
#      + syntax:   $ yarp priority-qos describe [value]
#        . value   It can be an integer (TOS decimal value), a DSCP class label (e.g. "AF11") or a level label (e.g. "CRIT")
#                  If omitted, the wholetable will be shown, etherwise, only the line corresponding to the specified value
#                  will be printed
#    - get             It returns the packet priority values of the connection
#      + syntax:       $ yarp priority-qos get <src-port> <des-port> [port] [format]
#        . src-port    It's the name of the source port of the selected connection (from now on "/w")
#        . des-port    It's the name of the destination port of the selected connection (from now on "/r")
#        . port        The port you want to get data from. The values can be "--src" for /w or
#                      "--des" for /r. If omitted, data from both the ports will be returned
#        . format      The wanted format to be used to represent the packet priority values. It can be
#                      "--dscp" if you want the value to be represented as DSCP classes labels. It can
#                      than be "--level" for QOS level representation or "--tos" to get the decimal
#                      tos value. If omitted the data will be returned as decimal TOS values
#    - set             It sets the packet priority value for a specific port.
#      + syntax        $ yarp priority-qos get <src-port> <des-port> <port> <value>
#        . src-port    As for "get", it is the name of /w
#        . des-port    As for "get", it is the name of /r
#        . port        The port to which send the new packet priority value. The values can be "--src" for /w or "--des" for /r
#        . value       The value to set. It can be a simple integer (it will be then interpreted as a decimal TOS value)
#                      It can then be "LABEL:value" where "LABEL" can be "DSCP", "LEVEL", or "TOS".
#                      In the first case, the value can be a DSCP class label (e.g. "AF11") or a decimal integer value
#                      that corresponds to a DSCP class (when not sure, use "describe" to get the info you need)
#                      In the second case, the value can be a level label (e.g. "HIGH") of a decimal integer value that
#                      corresponds to a packet priority level (when not sure, use "describe" to get the info you need).
#                      In the last case, "value" must be a decimal integer value)===";
    }
    else if(mode=="get" || mode=="set"){
        if(argc<2){
            yCError(COMPANION, "Get/set mode must have two ports involved in the connection as parameters (argc>=2)");
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
            std::vector<std::string> formats = {"dscp","level","tos"};
            std::string outputString;
            std::string warningString = "";
            if(argc==2){
                int srcTos = srcStyle.getPacketPriorityAsTOS();
                int desTos = desStyle.getPacketPriorityAsTOS();
                std::ostringstream streamOut;
                streamOut<<"(src (qos ((tos "<<srcTos<<")))) (des (qos ((tos "<<desTos<<"))))";
                outputString = streamOut.str();
            }
            else if(argc==3){
                std::vector<std::string>::iterator it = std::find(formats.begin(), formats.end(), std::string(argv[2]).substr(2));
                if (it != formats.end()){
                    int index = std::distance(formats.begin(), it);
                    std::string srcPP, srcFormat;
                    std::string desPP, desFormat;
                    switch(index){
                    case 0:{
                        auto srcDSCP = srcStyle.getPacketPriorityAsDSCP();
                        if(srcDSCP == QosStyle::DSCP_Undefined || srcDSCP == QosStyle::DSCP_Invalid){
                            warningString += "The source port packet priority cannot be expressed as a DSCP class";
                            srcPP = std::to_string(srcStyle.getPacketPriorityAsTOS());
                            srcFormat = "tos";
                        }
                        else {
                            srcPP = dscpClassesMap.at(srcDSCP);
                            srcFormat = "dscp";
                        }
                        auto desDSCP = desStyle.getPacketPriorityAsDSCP();
                        if(desDSCP == QosStyle::DSCP_Undefined || desDSCP == QosStyle::DSCP_Invalid){
                            warningString += "The destination port packet priority cannot be expressed as a DSCP class";
                            desPP = std::to_string(desStyle.getPacketPriorityAsTOS());
                            desFormat = "tos";
                        }
                        else {
                            desPP = dscpClassesMap.at(desDSCP);
                            desFormat = "dscp";
                        }
                        break;
                    }
                    case 1: {
                        auto srcLev = srcStyle.getPacketPriorityAsLevel();
                        if(srcLev == QosStyle::PacketPriorityUndefined || srcLev == QosStyle::PacketPriorityInvalid){
                            warningString += "The source port packet priority cannot be expressed as a Level";
                            srcPP = std::to_string(srcStyle.getPacketPriorityAsTOS());
                            srcFormat = "tos";
                        }
                        else {
                            srcPP = levelClassesMap.at(srcLev);
                            srcFormat = "dscp";
                        }
                        auto desLev = desStyle.getPacketPriorityAsLevel();
                        if(desLev == QosStyle::PacketPriorityUndefined || desLev == QosStyle::PacketPriorityInvalid){
                            warningString += "The destination port packet priority cannot be expressed as a Level";
                            desPP = std::to_string(desStyle.getPacketPriorityAsTOS());
                            desFormat = "tos";
                        }
                        else {
                            desPP = levelClassesMap.at(desLev);
                            desFormat = "dscp";
                        }
                        break;
                    }
                    case 2: {
                        srcPP = std::to_string(srcStyle.getPacketPriorityAsTOS());
                        srcFormat = "tos";
                        desPP = std::to_string(desStyle.getPacketPriorityAsTOS());
                        desFormat = "tos";
                        break;
                    }
                    }
                    std::ostringstream streamOut;
                    streamOut<<"(src (qos (("<<srcFormat<<" "<<srcPP<<")))) (des (qos (("<<desFormat<<" "<<desPP<<"))))";
                    outputString = streamOut.str();
                }
                else{
                    std::string port = std::string(argv[2]).substr(2);
                    int priority;
                    if(port == "src"){
                        priority = srcStyle.getPacketPriorityAsTOS();
                    }
                    else if (port == "des"){
                        priority = desStyle.getPacketPriorityAsTOS();
                    }
                    else{
                        yCError(COMPANION, "Wrong value. Do not know what %s is",argv[2]);
                        return 1;
                    }
                    std::ostringstream streamOut;
                    streamOut<<"("<<port<<" (qos ((tos "<<priority<<"))))";
                    outputString = streamOut.str();
                }
            }
            else if(argc==4){
                bool foundPort = false;
                bool foundFormat = false;
                std::string port, format;
                std::string argv2 = std::string(argv[2]).substr(2);
                std::string argv3 = std::string(argv[3]).substr(2);
                std::vector<std::string>::iterator it2 = std::find(formats.begin(), formats.end(), argv2);
                std::vector<std::string>::iterator it3 = std::find(formats.begin(), formats.end(), argv3);
                int index;
                if(it2 == formats.end() && it3 != formats.end()){
                    format = argv3;
                    index = std::distance(formats.begin(), it3);
                    foundFormat = true;
                    if(argv2 == "src" || argv2 == "des"){
                        port = argv2;
                        foundPort = true;
                    }
                }
                else if(it2 != formats.end() && it3 == formats.end()){
                    format = argv2;
                    index = std::distance(formats.begin(), it2);
                    foundFormat = true;
                    if(argv3 == "src" || argv3 == "des"){
                        port = argv3;
                        foundPort = true;
                    }
                }
                else {
                    foundPort = (argv3 == "src" || argv3 == "des") || (argv2 == "src" || argv2 == "des");
                }

                if(foundPort && foundFormat){
                    std::string priority;
                    switch(index){
                    case 0:{
                        auto dscp = port == "src" ? srcStyle.getPacketPriorityAsDSCP() : desStyle.getPacketPriorityAsDSCP();
                        if(dscp == QosStyle::DSCP_Undefined || dscp == QosStyle::DSCP_Invalid){
                            warningString += "The port packet priority cannot be expressed as a DSCP class";
                            priority = std::to_string(port == "src" ? srcStyle.getPacketPriorityAsTOS() : desStyle.getPacketPriorityAsTOS());
                            format = "tos";
                        }
                        else {
                            priority = dscpClassesMap.at(dscp);
                        }
                        break;
                    }
                    case 1: {
                        auto level = port == "src" ? srcStyle.getPacketPriorityAsLevel() : desStyle.getPacketPriorityAsLevel();
                        if(level == QosStyle::PacketPriorityUndefined || level == QosStyle::PacketPriorityInvalid){
                            warningString += "The port packet priority cannot be expressed as a Level";
                            priority = std::to_string(port == "src" ? srcStyle.getPacketPriorityAsTOS() : desStyle.getPacketPriorityAsTOS());
                            format = "tos";
                        }
                        else {
                            priority = levelClassesMap.at(level);
                        }
                        break;
                    }
                    case 2: {
                        priority = std::to_string(port == "src" ? srcStyle.getPacketPriorityAsTOS() : desStyle.getPacketPriorityAsTOS());
                        break;
                    }
                    }
                    std::ostringstream streamOut;
                    streamOut<<"("<<port<<" (qos (("<<format<<" "<<priority<<"))))";
                    outputString = streamOut.str();
                }
                else{
                    yCError(COMPANION, "%s",foundPort ? "Port found but wrong format" : "Format ok, but wrong port selected");
                    return 1;
                }
            }
            if(warningString.length()>0){
                yCWarning(COMPANION, "%s", warningString.c_str());
            }
            yCInfo(COMPANION, "%s",outputString.c_str());
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
            std::vector<std::string> labels = {"DSCP:","LEVEL:","TOS:"};
            std::string label = "";
            size_t pos = std::string::npos;
            for(auto l: labels){
                pos = value.find(l);
                if(pos!=std::string::npos){
                    label = l;
                    break;
                }
            }
            if(label == ""){
                try{
                    int tos = stoi(value);
                    forTest.setPacketPrioritybyTOS(tos);
                    if(forTest.getPacketPriorityAsTOS() != tos){
                        yCInfo(COMPANION, "Provided packet priority was: %d But has been set to: %d",tos,forTest.getPacketPriorityAsTOS());
                    }
                    toSet.setPacketPrioritybyTOS(tos);
                }
                catch (std::invalid_argument const &e)
                {
                    QosStyle::PacketPriorityDSCP equivalent = QosStyle::DSCP_Invalid;
                    for (std::map<QosStyle::PacketPriorityDSCP,std::string>::const_iterator it=dscpClassesMap.begin(); it!=dscpClassesMap.end(); ++it)
                    {
                       if(it->second == value){
                           equivalent = it->first;
                           break;
                       }
                    }
                    if(equivalent != QosStyle::DSCP_Invalid && equivalent != QosStyle::DSCP_Undefined){
                        toSet.setPacketPriorityByDscp(equivalent);
                    }
                    else{
                        QosStyle::PacketPriorityLevel equivalent = QosStyle::PacketPriorityInvalid;
                        for (std::map<QosStyle::PacketPriorityLevel,std::string>::const_iterator it=levelClassesMap.begin(); it!=levelClassesMap.end(); ++it)
                        {
                           if(it->second == value){
                               equivalent = it->first;
                               break;
                           }
                        }
                        if(equivalent != QosStyle::PacketPriorityInvalid && equivalent != QosStyle::PacketPriorityUndefined){
                            toSet.setPacketPriorityByLevel(equivalent);
                        }
                        else{
                            yCError(COMPANION, "You provided an invalid value (%s). Please try again",value.c_str());
                            return 1;
                        }
                    }
                }
                catch (std::out_of_range const &e)
                {
                    yCError(COMPANION, "Integer overflow: std::out_of_range thrown");
                    return 1;
                }
            }
            else {
                std::string number = value.erase(pos,label.length());
                try{
                    int tempVal = stoi(number);
                    if(label == "DSCP:"){
                        QosStyle::PacketPriorityDSCP val = static_cast<QosStyle::PacketPriorityDSCP>(tempVal);
                        forTest.setPacketPriorityByDscp(val);
                        if(forTest.getPacketPriorityAsDSCP() == QosStyle::DSCP_Undefined){
                            yCError(COMPANION, "Provided packet priority was %d which has been labelled to: %s. Please check again", val,dscpClassesMap.at(forTest.getPacketPriorityAsDSCP()).c_str());
                            return 1;
                        }
                        toSet.setPacketPriorityByDscp(val);
                    }
                    else if(label == "LEVEL:"){
                        QosStyle::PacketPriorityLevel val = static_cast<QosStyle::PacketPriorityLevel>(tempVal);
                        forTest.setPacketPriorityByLevel(val);
                        if(forTest.getPacketPriorityAsLevel() == QosStyle::PacketPriorityUndefined){
                            yCError(COMPANION, "Provided packet priority was %d which has been labelled to: %s. Please check again",val,levelClassesMap.at(forTest.getPacketPriorityAsLevel()).c_str());
                            return 1;
                        }
                        toSet.setPacketPriorityByLevel(val);
                    }
                    else if(label == "TOS:"){
                        forTest.setPacketPrioritybyTOS(tempVal);
                        if(forTest.getPacketPriorityAsTOS() != tempVal){
                            yCInfo(COMPANION, "Provided packet priority was %d but has been set to: %d",tempVal,forTest.getPacketPriorityAsTOS());
                        }
                        toSet.setPacketPrioritybyTOS(tempVal);
                    }
                }
                catch (std::invalid_argument const &e)
                {
                    if(label == "DSCP:"){
                        QosStyle::PacketPriorityDSCP equivalent = QosStyle::DSCP_Invalid;
                        for (std::map<QosStyle::PacketPriorityDSCP,std::string>::const_iterator it=dscpClassesMap.begin(); it!=dscpClassesMap.end(); ++it)
                        {
                           if(it->second == number){
                               equivalent = it->first;
                               break;
                           }
                        }
                        if(equivalent != QosStyle::DSCP_Invalid && equivalent != QosStyle::DSCP_Undefined){
                            toSet.setPacketPriorityByDscp(equivalent);
                        }
                        else{
                            yCError(COMPANION, "You provided an invalid value (%s). Please try again",number.c_str());
                            return 1;
                        }
                    }
                    else if(label == "LEVEL:"){
                        QosStyle::PacketPriorityLevel equivalent = QosStyle::PacketPriorityInvalid;
                        for (std::map<QosStyle::PacketPriorityLevel,std::string>::const_iterator it=levelClassesMap.begin(); it!=levelClassesMap.end(); ++it)
                        {
                           if(it->second == number){
                               equivalent = it->first;
                               break;
                           }
                        }
                        if(equivalent != QosStyle::PacketPriorityInvalid && equivalent != QosStyle::PacketPriorityUndefined){
                            toSet.setPacketPriorityByLevel(equivalent);
                        }
                        else{
                            yCError(COMPANION, "You provided an invalid value (%s). Please try again",number.c_str());
                            return 1;
                        }
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
        yCError(COMPANION, "Unspupported mode (type \"yarp priority-qos --help\" to get help)");
    }
    return 0;
}
