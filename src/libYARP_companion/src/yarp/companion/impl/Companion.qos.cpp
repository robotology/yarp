/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/companion/impl/Companion.h>
#include <yarp/companion/yarpcompanion.h>

#include <yarp/os/Network.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

using namespace yarp::companion::impl;
using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp;

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
const std::map<std::string,int> schedBrowser = {{"SCHED_OTHER",1},{"0",1},
                                                {"SCHED_FIFO",2},{"1",2},
                                                {"SCHED_RR",3},{"2",3}};
const std::map<int,std::string> policyLabels = {{-1,"INVALID"},{0,"SCHED_OTHER"},{1,"SCHED_FIFO"},{2,"SCHED_RR"}};
const std::map<int,std::vector<int>> priorityLimitsMap = {{0,{0,0}},{1,{1,99}},{2,{1,99}}};


const std::string dscpHeader = "| DSCP  | DSCP   | DSCP  | DSCP  | TOS   | ToS   | ToS      | ToS   | ToS   | ToS   | ToS       |"
                          " ToS         | ToS                 | YARP  |\n| class | (bin)  | (hex) | (dec) | (dec) | (hex) | "
                          "(bin)    | Prec. | Prec. | Delay | Throghput | Reliability | Str.                | Level |\n|    "
                          "   |        |       |       |       |       |          | (bin) | (dec) | Flag  | Flag      | Flag"
                          "        | Format              |       |\n|-------|--------|-------|-------|-------|-------|------"
                          "----|-------|-------|-------|-----------|-------------|---------------------|-------|";
const std::string schedHeader = "| Policy label | Policy (dec) | Priority min (dec) | Priority max (dec) |\n"
                                "|--------------|--------------|--------------------|--------------------|";
const  std::map<int,std::string> dscpTable = {{0,dscpHeader},
                                   {1,"| CS0   | 000000 | 0×00  | 0     | 0     | 0×00  | 00000000 | 000   | 0     | 0     | 0         | 0           | -                   | NORM  |"},
                                   {2,"| CS1   | 001000 | 0×08  | 8     | 32    | 0×20  | 00100000 | 001   | 1     | 0     | 0         | 0           | Priority            | -     |"},
                                   {3,"| CS2   | 010000 | 0×10  | 16    | 64    | 0×40  | 01000000 | 010   | 2     | 0     | 0         | 0           | Immediate           | -     |"},
                                   {4,"| CS3   | 011000 | 0×18  | 24    | 96    | 0×60  | 01100000 | 011   | 3     | 0     | 0         | 0           | Flash               | -     |"},
                                   {5,"| CS4   | 100000 | 0×20  | 32    | 128   | 0×80  | 10000000 | 100   | 4     | 0     | 0         | 0           | FlashOverride       | -     |"},
                                   {6,"| CS5   | 101000 | 0×28  | 40    | 160   | 0×A0  | 10100000 | 101   | 5     | 0     | 0         | 0           | Critical            | -     |"},
                                   {7,"| CS6   | 110000 | 0×30  | 48    | 192   | 0×C0  | 11000000 | 110   | 6     | 0     | 0         | 0           | Internetworkcontrol | -     |"},
                                   {8,"| CS7   | 111000 | 0×38  | 56    | 224   | 0×E0  | 11100000 | 111   | 7     | 0     | 0         | 0           | Networkcontrol      | -     |"},
                                   {9,"| AF11  | 001010 | 0×0A  | 10    | 40    | 0×28  | 00101000 | 001   | 1     | 0     | 1         | 0           | Priority            | LOW   |"},
                                   {10,"| AF12  | 001100 | 0×0C  | 12    | 48    | 0×30  | 00110000 | 001   | 1     | 1     | 0         | 0           | Priority            | -     |"},
                                   {11,"| AF13  | 001110 | 0×0E  | 14    | 56    | 0×38  | 00111000 | 001   | 1     | 1     | 1         | 0           | Priority            | -     |"},
                                   {12,"| AF21  | 010010 | 0×12  | 18    | 72    | 0×48  | 01001000 | 010   | 2     | 0     | 1         | 0           | Immediate           | -     |"},
                                   {13,"| AF22  | 010100 | 0×14  | 20    | 80    | 0×50  | 01010000 | 010   | 2     | 1     | 0         | 0           | Immediate           | -     |"},
                                   {14,"| AF23  | 010110 | 0×16  | 22    | 88    | 0×58  | 01011000 | 010   | 2     | 1     | 1         | 0           | Immediate           | -     |"},
                                   {15,"| AF31  | 011010 | 0×1A  | 26    | 104   | 0×68  | 01101000 | 011   | 3     | 0     | 1         | 0           | Flash               | -     |"},
                                   {16,"| AF32  | 011100 | 0×1C  | 28    | 112   | 0×70  | 01110000 | 011   | 3     | 1     | 0         | 0           | Flash               | -     |"},
                                   {17,"| AF33  | 011110 | 0×1E  | 30    | 120   | 0×78  | 01111000 | 011   | 3     | 1     | 1         | 0           | Flash               | -     |"},
                                   {18,"| AF41  | 100010 | 0×22  | 34    | 136   | 0×88  | 10001000 | 100   | 4     | 0     | 1         | 0           | FlashOverride       | -     |"},
                                   {19,"| AF42  | 100100 | 0×24  | 36    | 144   | 0×90  | 10010000 | 100   | 4     | 1     | 0         | 0           | FlashOverride       | HIGH  |"},
                                   {20,"| AF43  | 100110 | 0×26  | 38    | 152   | 0×98  | 10011000 | 100   | 4     | 1     | 1         | 0           | FlashOverride       | -     |"},
                                   {21,"| VA    | 101100 | 0×2C  | 44    | 176   | 0×B0  | 10110000 | 101   | 5     | 1     | 0         | 0           | Critical            | CRIT  |"},
                                   {22,"| EF    | 101110 | 0×2E  | 46    | 184   | 0×B8  | 10111000 | 101   | 5     | 1     | 1         | 0           | Critical            | -     |"}};
const std::map<int,std::string> schedTable = {{0,schedHeader},
                                              {1,"| SCHED_OTHER  | 0            | 0                  | 0                  |"},
                                              {2,"| SCHED_FIFO   | 1            | 1                  | 99                 |"},
                                              {3,"| SCHED_RR     | 2            | 1                  | 99                 |"}};


const std::string printTable( std::map<int,std::string> inputTable,int index){
    std::string toReturn = "\n";
    if(index==0){
        for (auto const& line : inputTable)
        {
            toReturn += line.second+"\n";
        }
    }
    else{
        toReturn += inputTable[0]+"\n"+inputTable[index]+"\n";
    }

    return toReturn;
}

int Companion::cmdPriorityQos(int argc, char *argv[]){
    if(argc==0){
        printf("Not enough parameters/options. If not sure on how to use the command, please type \"yarp priority-qos --help\" for instructions\n");
        return 1;
    }
    std::string mode;
    mode = std::string(argv[0]);
    argc--;
    argv++;
    if(mode=="describe"){
        if(argc==0){
            printf("%s\n",printTable(dscpTable,0).c_str());
        }
        else{
            std::string toDescribe = std::string(argv[0]);
            if(dscpBrowser.count(toDescribe)!=0){
                printf("%s\n",printTable(dscpTable,dscpBrowser.at(toDescribe)).c_str());
            }
        }
    } // 22 spaces
    else if (mode=="--help"){
        printf("## yarp priority-qos: a companion command for getting/setting packet priority for a connection\n"
               "# Syntax:\n\n#    $ yarp priority-qos <mode> [options...]\n\n# <mode> argument can be:\n#    - describe    "
               "It shows a table with useful info about the packet priorities values\n#      + syntax:   $ yarp "
               "priority-qos describe [value]\n#        . value   It can be an integer (TOS decimal value), a DSCP class"
               " label (e.g. \"AF11\") or a level label (e.g. \"CRIT\")\n#                  If omitted, the whole"
               "table will be shown, etherwise, only the line corresponding to the specified value\n#"
               "                  will be printed\n#    - get             It returns the packet priority values of the connection"
               "\n#      + syntax:       $ yarp priority-qos get <src-port> <des-port> [port] [format]\n#        . src-port    "
               "It's the name of the source port of the selected connection (from now on \"/w\")\n#        . des-port    "
               "It's the name of the destination port of the selected connection (from now on \"/r\")\n"
               "#        . port        The port you want to get data from. The values can be \"--src\" for /w or\n"
               "#                      \"--des\" for /r. If omitted, data from both the ports will be returned\n"
               "#        . format      The wanted format to be used to represent the packet priority values. It can be\n"
               "#                      \"--dscp\" if you want the value to be represented as DSCP classes labels. It can\n"
               "#                      than be \"--level\" for QOS level representation or \"--tos\" to get the decimal\n"
               "#                      tos value. If omitted the data will be returned as decimal TOS values\n"
               "#    - set             It sets the packet priority value for a specific port.\n"
               "#      + syntax        $ yarp priority-qos get <src-port> <des-port> <port> <value>\n"
               "#        . src-port    As for \"get\", it is the name of /w\n"
               "#        . des-port    As for \"get\", it is the name of /r\n"
               "#        . port        The port to which send the new packet priority value. The values can be \"--src\" for /w or \"--des\" for /r\n"
               "#        . value       The value to set. It can be a simple integer (it will be then interpreted as a decimal TOS value)\n"
               "#                      It can then be \"LABEL:value\" where \"LABEL\" can be \"DSCP\", \"LEVEL\", or \"TOS\".\n"
               "#                      In the first case, the value can be a DSCP class label (e.g. \"AF11\") or a decimal integer value\n"
               "#                      that corresponds to a DSCP class (when not sure, use \"describe\" to get the info you need)\n"
               "#                      In the second case, the value can be a level label (e.g. \"HIGH\") of a decimal integer value that\n"
               "#                      corresponds to a packet priority level (when not sure, use \"describe\" to get the info you need).\n"
               "#                      In the last case, \"value\" must be a decimal integer value\n");
    }
    else if(mode=="get" || mode=="set"){
        if(argc<2){
            printf("Get/set mode must have two ports involved in the connection as parameters (argc>=2)\n");
        }
        std::string src, des;
        src = std::string(argv[0]);
        des = std::string(argv[1]);
        if(!Network::exists(src) || !Network::exists(des)){
            printf("%s not found. Please check again\n", (!Network::exists(src) ? src : (!Network::exists(src) == !Network::exists(des) ? "Neither "+src+" nor "+des : des)).c_str());
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
                            warningString += "The source port packet priority cannot be expressed as a DSCP class\n";
                            srcPP = std::to_string(srcStyle.getPacketPriorityAsTOS());
                            srcFormat = "tos";
                        }
                        else {
                            srcPP = dscpClassesMap.at(srcDSCP);
                            srcFormat = "dscp";
                        }
                        auto desDSCP = desStyle.getPacketPriorityAsDSCP();
                        if(desDSCP == QosStyle::DSCP_Undefined || desDSCP == QosStyle::DSCP_Invalid){
                            warningString += "The destination port packet priority cannot be expressed as a DSCP class\n";
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
                            warningString += "The source port packet priority cannot be expressed as a Level\n";
                            srcPP = std::to_string(srcStyle.getPacketPriorityAsTOS());
                            srcFormat = "tos";
                        }
                        else {
                            srcPP = levelClassesMap.at(srcLev);
                            srcFormat = "dscp";
                        }
                        auto desLev = desStyle.getPacketPriorityAsLevel();
                        if(desLev == QosStyle::PacketPriorityUndefined || desLev == QosStyle::PacketPriorityInvalid){
                            warningString += "The destination port packet priority cannot be expressed as a Level\n";
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
                        printf("Wrong value. Do not know what %s is\n",argv[2]);
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
                            warningString += "The port packet priority cannot be expressed as a DSCP class\n";
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
                            warningString += "The port packet priority cannot be expressed as a Level\n";
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
                    printf("%s\n",foundPort ? "Port found but wrong format" : "Format ok, but wrong port selected");
                    return 1;
                }
            }
            if(warningString.length()>0){
                printf("WARNING:\n%s\n",warningString.c_str());
            }
            printf("%s\n",outputString.c_str());
        }
        else{
            if(argc<4){
                printf("Set mode must have the port to apply the new parameter to and the value of the packet priority as parameters (argc>=4)\n");
                return 1;
            }
            std::string argv2 = std::string(argv[2]);
            std::string argv3 = std::string(argv[3]);
            std::string port = argv2 == "--src" || argv2 == "--des" ? argv2 : (argv3 == "--src" || argv3 == "--des" ? argv3 : "none");
            if(port == "none"){
                printf("You haven\'t specified a port to which to apply the changes\n");
                return 1;
            }
            QosStyle& toSet = port == "--src" ? srcStyle : desStyle;
            QosStyle forTest;
            std::string value = argv2 != "--src" && argv2 != "--des" ? argv2 : (argv3 != "--src" && argv3 != "--des" ? argv3 : "none");
            if(value == "none"){
                printf("You, somehow, specified two ports instead of a port and a value. Please try again\n");
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
                        printf("Provided packet priority was: %d But has been set to: %d\n",tos,forTest.getPacketPriorityAsTOS());
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
                            printf("You provided an invalid value (%s). Please try again\n",value.c_str());
                            return 1;
                        }
                    }
                }
                catch (std::out_of_range const &e)
                {
                    printf("Integer overflow: std::out_of_range thrown\n");
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
                            printf("Provided packet priority was %d which has been labelled to: %s. Please check again\n",val,dscpClassesMap.at(forTest.getPacketPriorityAsDSCP()).c_str());
                            return 1;
                        }
                        toSet.setPacketPriorityByDscp(val);
                    }
                    else if(label == "LEVEL:"){
                        QosStyle::PacketPriorityLevel val = static_cast<QosStyle::PacketPriorityLevel>(tempVal);
                        forTest.setPacketPriorityByLevel(val);
                        if(forTest.getPacketPriorityAsLevel() == QosStyle::PacketPriorityUndefined){
                            printf("Provided packet priority was %d which has been labelled to: %s. Please check again\n",val,levelClassesMap.at(forTest.getPacketPriorityAsLevel()).c_str());
                            return 1;
                        }
                        toSet.setPacketPriorityByLevel(val);
                    }
                    else if(label == "TOS:"){
                        forTest.setPacketPrioritybyTOS(tempVal);
                        if(forTest.getPacketPriorityAsTOS() != tempVal){
                            printf("Provided packet priority was %d but has been set to: %d\n",tempVal,forTest.getPacketPriorityAsTOS());
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
                            printf("You provided an invalid value (%s). Please try again\n",number.c_str());
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
                            printf("You provided an invalid value (%s). Please try again\n",number.c_str());
                            return 1;
                        }
                    }

                }
                catch (std::out_of_range const &e)
                {
                    printf( "Integer overflow: std::out_of_range thrown\n");
                    return 1;
                }
            }
            Network::setConnectionQos(src,des,srcStyle,desStyle);
        }
    }
    else{
        printf("Unspupported mode (type \"yarp priority-qos --help\" to get help)\n");
    }
    return 0;
}

int Companion::cmdPrioritySched(int argc, char *argv[]){
    if(argc==0){
        printf("Not enough parameters/options. If not sure on how to use the command, please type \"yarp priority-sched --help\" for instructions\n");
        return 1;
    }

    std::string mode;
    mode = std::string(argv[0]);
    argc--;
    argv++;
    if(mode=="--help"){
        printf("\n## yarp priority-sched: a companion command for getting/setting thread priority and policy priority for a connection\n"
               "# Syntax:\n\n#    $ yarp priority-sched <mode> [options...]\n\n# <mode> argument can be:\n#    - describe    "
               "It shows a table with useful info about the thread policies values and their ranges of priorities\n"
               "#      + syntax:   $ yarp priority-sched describe [value]\n"
               "#        . value   It can be an integer (policy int value), a policy label (e.g. \"SCHED_RR\")\n"
               "#                  If omitted, the whole table will be shown, etherwise, only the line corresponding to the specified value\n#"
               "#                  will be printed\n"
               "#    - get             It returns the thread policy and/or priority values of the connection\n"
               "#      + syntax:       $ yarp priority-sched get <src-port> <des-port> [port] [param]\n"
               "#        . src-port    It's the name of the source port of the selected connection (from now on \"/w\")\n"
               "#        . des-port    It's the name of the destination port of the selected connection (from now on \"/r\")\n"
               "#        . port        The port you want to get data from. The values can be \"--src\" for /w or\n"
               "#                      \"--des\" for /r. If omitted, data from both the ports will be returned\n"
               "#        . param       The wanted param you want to get. It can be \"--policy\" if you want to get the thread\n"
               "#                      policy as an integer. Than, it can be \"--policy-label\" if you want the same value but\n"
               "#                      represented as a label. The last possible value is \"--priority\" if you want to get info\n"
               "#                      about the thread priority. If \"param\" is omitted, the command will return both\n"
               "#                      parameters a integers\n"
               "#    - set             It sets the thread policy or priority value for a specific port.\n"
               "#      + syntax        $ yarp priority-sched get <src-port> <des-port> <port> <value>\n"
               "#        . src-port    As for \"get\", it is the name of /w\n"
               "#        . des-port    As for \"get\", it is the name of /r\n"
               "#        . port        The port to which send the new policy/priority value. The values can be \"--src\" for /w or \"--des\" for /r\n"
               "#        . value       The value to set. t can then be \"LABEL:value\" where \"LABEL\" can be \"PRT\" or \"POL\".\n"
               "#                      In the first case, the value can be a policy label (e.g. \"SCHED_FIFO\") or a decimal integer value between 0 and 2\n"
               "#                      If the currently set priority is outside the newly set policy, the command will warn you and then set\n"
               "#                      as priority the minimum allowed value for the new policy. In the second case, the value can only be \n"
               "#                      the priority integer value. If this value is outside the allowed priority range for the currently set \n"
               "#                      policy, the command will not set it and return an error message.\n\n");
    }
    else if(mode=="describe"){
        if(argc==0){
            printf("%s\n",printTable(schedTable,0).c_str());
        }
        else{
            std::string toDescribe = std::string(argv[0]);
            if(schedBrowser.count(toDescribe)!=0){
                printf("%s\n",printTable(schedTable,schedBrowser.at(toDescribe)).c_str());
            }
        }
    }
    else if(mode=="get" || mode=="set"){
        if(argc<2){
            printf("Get/set mode must have two ports involved in the connection as parameters (argc>=2)\n");
            return 1;
        }
        std::string src, des;
        src = std::string(argv[0]);
        des = std::string(argv[1]);
        if(!Network::exists(src) || !Network::exists(des)){
            printf("%s not found. Please check again\n", (!Network::exists(src) ? src : (!Network::exists(src) == !Network::exists(des) ? "Neither "+src+" nor "+des : des)).c_str());
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
                printf("(src (sched ((policy %d) (priority %d))))(des (sched ((policy %d) (priority %d))))\n",srcPol,srcPrt,desPol,desPrt);
            }
            else if(argc==3){
                std::string argv2 = std::string(argv[2]).substr(2);
                std::vector<std::string>::iterator it = std::find(labels.begin(), labels.end(), argv2);
                if(it!=labels.end()){
                    int index = std::distance(labels.begin(), it);
                    switch(index){
                    case 0: {
                        printf("(src (sched ((policy %d))))(des (sched ((policy %d))))\n",srcPol,desPol);
                        break;
                    }
                    case 1: {
                        printf("(src (sched ((policy %s))))(des (sched ((policy %s))))\n",policyLabels.at(srcPol).c_str(),policyLabels.at(desPol).c_str());
                        break;
                    }
                    case 2: {
                        printf("(src (sched ((priority %d))))(des (sched ((priority %d))))\n",srcPrt,desPrt);
                        break;
                    }
                    }
                }
                else{
                    if(argv2=="src"){
                        printf("(src (sched ((policy %d) (priority %d))))\n",srcPol,srcPrt);
                    }
                    else if(argv2=="des"){
                        printf("(des (sched ((policy %d) (priority %d))))\n",desPol,desPrt);
                    }
                    else{
                        printf("Wrong value. Do not know what %s is\n",argv[2]);
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
                    printf("%s\n",streamOut.str().c_str());
                }
                else{
                    printf("%s\n",foundPort ? "Port found but wrong parameter" : "Parameter ok, but wrong port selected");
                    return 1;
                }
            }
        }
        else{
            if(argc<4){
                printf("Set mode must have the port to apply the new parameter to and the value of the packet priority as parameters (argc>=4)\n");
                return 1;
            }
            std::string argv2 = std::string(argv[2]);
            std::string argv3 = std::string(argv[3]);
            std::string port = argv2 == "--src" || argv2 == "--des" ? argv2 : (argv3 == "--src" || argv3 == "--des" ? argv3 : "none");
            if(port == "none"){
                printf("You haven\'t specified a port to which to apply the changes\n");
                return 1;
            }
            QosStyle& toSet = port == "--src" ? srcStyle : desStyle;
            QosStyle forTest;
            std::string value = argv2 != "--src" && argv2 != "--des" ? argv2 : (argv3 != "--src" && argv3 != "--des" ? argv3 : "none");
            if(value == "none"){
                printf("You, somehow, specified two ports instead of a port and a value. Please try again\n");
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
                printf("The label used for the data has not been recognised. Try again (type \"yarp priority-sched --help\" for info)");
                return 1;
            }
            else{
                std::string number = value.erase(pos,label.length());
                try {
                    int tempVal = stoi(number);
                    if(label=="POL:"){
                        if(tempVal<0 || tempVal>2){
                            printf("Invalid policy value. Policy must be a value between 0 and 2\n");
                            return 1;
                        }
                        if(toSet.getThreadPriority()<priorityLimitsMap.at(tempVal)[0] || toSet.getThreadPriority()>priorityLimitsMap.at(tempVal)[1]){
                            printf("The currently set priority is not compatible with selected policy. Therefor it will be set to %d\n",priorityLimitsMap.at(tempVal)[0]);
                            toSet.setThreadPriority(priorityLimitsMap.at(tempVal)[0]);
                        }
                        toSet.setThreadPolicy(tempVal);
                    }
                    else{
                        int policy = toSet.getThreadPolicy();
                        if(policy == -1){
                            printf("The policy for this port is set to an invalid value. Change the policy before setting the priority value");
                            return -1;
                        }
                        if(tempVal<priorityLimitsMap.at(policy)[0] || tempVal>priorityLimitsMap.at(policy)[1]){
                            printf("Invalid priority value. Priority must be a value between %d and %d\n",priorityLimitsMap.at(policy)[0],priorityLimitsMap.at(policy)[1]);
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
                                printf("The currently set priority is not compatible with selected policy. Therefor it will be set to %d\n",priorityLimitsMap.at(equivalent)[0]);
                                toSet.setThreadPriority(priorityLimitsMap.at(equivalent)[0]);
                            }
                            toSet.setThreadPolicy(equivalent);
                        }
                        else{
                            printf("You provided an invalid value (%s). Please try again\n",number.c_str());
                            return 1;
                        }
                    }
                    else{
                        printf("Priority can only be a numerical value. Please try again\n");
                        return 1;
                    }
                }
                catch (std::out_of_range const &e)
                {
                    printf( "Integer overflow: std::out_of_range thrown\n");
                    return 1;
                }
            }
            Network::setConnectionQos(src,des,srcStyle,desStyle);
        }
    }
    else{
        printf("Unspupported mode (type \"yarp priority-sched --help\" to get help)\n");
    }
    return 0;
}
