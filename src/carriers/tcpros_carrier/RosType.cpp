// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <string.h>

#include "RosType.h"

#include <vector>

#include <sys/stat.h>

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_unistd.h>
#  include <ace/OS_NS_sys_wait.h>
#else
#  include <unistd.h>
#  include <sys/wait.h>
#  ifndef ACE_OS
#    define ACE_OS
#  endif
#endif
#include <stdlib.h>

using namespace std;

vector<string> normalizedMessage(const string& line) {
    vector<string> all;
    string result;
    bool quote = false;
    bool pending = false;
    for (int i=0; i<(int)line.length(); i++) {
        char ch = line[i];
        if (ch=='\"') {
            quote = !quote;
            result += ch;
            pending = true;
        } else if (quote) {
            result += ch;
            pending = true;
        } else if (ch==' '||ch=='\r'||ch=='\t') {
            if (pending) {
                if (result[0]=='#') return all;
                all.push_back(result);
                pending = false;
            }
            result = "";
        } else {
            result += ch;
            pending = true;
        }
    }
    if (pending&&result!="") {
        if (result[0]=='#') return all;
        all.push_back(result);
        pending = false;
    }
    return all;
}

#define HELPER(x) (*((std::vector<RosType> *)(x)))

RosType::RosTypes::RosTypes() {
    system_resource = new std::vector<RosType>();
    if (!system_resource) {
        fprintf(stderr,"Failed to allocated storage for ros types\n");
        exit(1);
    }
}

RosType::RosTypes::~RosTypes() {
    delete &HELPER(system_resource);
    system_resource = NULL;
}

RosType::RosTypes::RosTypes(const RosTypes& alt) {
    system_resource = new std::vector<RosType>();
    if (!system_resource) {
        fprintf(stderr,"Failed to allocated storage for ros types\n");
        exit(1);
    }
    HELPER(system_resource) = HELPER(alt.system_resource);
}

const RosType::RosTypes& RosType::RosTypes::operator=(const RosTypes& alt) {
    HELPER(system_resource) = HELPER(alt.system_resource);
    return *this;
}
        


void RosType::RosTypes::clear() {
    HELPER(system_resource).clear();
}

void RosType::RosTypes::push_back(const RosType& t) {
    HELPER(system_resource).push_back(t);
}

size_t RosType::RosTypes::size() {
    return HELPER(system_resource).size();
}

RosType& RosType::RosTypes::operator[](int i) {
    return HELPER(system_resource)[i];
}


bool RosType::read(const char *tname, RosTypeSearch& env, RosTypeCodeGen& gen,
                   int nesting) {
    string indent = "";
    for (int i=0; i<nesting; i++) {
        indent += "  ";
    }
    //printf("Checking %s\n", tname);
    isValid = false;
    isArray = false;
    isPrimitive = false;
    rosType = "";
    rosName = "";
    subRosType.clear();
    
    string base = tname;
    rosType = base;
    if (base.length()==0) return false;
    if (base[base.length()-1]==']') {
        char ch = '\0';
        if (base.length()>=2) {
            ch = base[base.length()-2];
        }
        if (ch!='[') {
            fprintf(stderr,"dodgy array? %s\n", base.c_str());
            return false;
        }
        isArray = true;
        base = base.substr(0,base.length()-2);
        rosType = base;
    }

    if (base[0]>='a'&&base[0]<='z'&&base.find("/")==string::npos) {
        if (base=="time") {
            if (gen.hasNativeTimeClass()) {
                isPrimitive = true;
            } else {
                rosType = "TickTime";
                isPrimitive = false;
                RosType t1;
                t1.rosType = "uint32";
                t1.rosName = "sec";
                t1.isPrimitive = true;
                t1.isValid = true;
                subRosType.push_back(t1);
                RosType t2;
                t2.rosType = "uint32";
                t2.rosName = "nsec";
                t2.isPrimitive = true;
                t2.isValid = true;
                subRosType.push_back(t2);
            }
        } else {
            isPrimitive = true;
        }
        isValid = true;
        return true;
    }

    bool ok = true;
    string path = env.findFile(base.c_str());
  
    FILE *fin = fopen(path.c_str(),"r");
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open %s\n", path.c_str());
        return false;
    }

    fprintf(stderr,"[type]%s BEGIN %s\n", indent.c_str(), path.c_str());
    char *result = NULL;
    txt = "";
    do {
        char buf[2048];
        result = fgets(buf,sizeof(buf),fin);
        if (result==NULL) break;
        txt += "//   ";
        txt += result;
        for (int i=0; i<(int)strlen(result); i++) {
            if (result[i]=='\n') {
                result[i] = '\0';
                break;
            }
        }
        string row = result;
        vector<string> msg = normalizedMessage(row);
        if (msg.size()==0) { continue; }
        if (msg.size()>2) {
            if (msg[2]=="=") {
                printf("Not worrying about: %s\n", row.c_str());
                continue;
            }
        }
        if (msg.size()!=2) {
            if (msg.size()>0) {
                if (msg[0][0]!='[') {
                    fprintf(stderr,"[type] skip %s\n", row.c_str());
                    ok = false;
                }
            }
            continue;
        }
        string t = msg[0];
        string n = msg[1];
        fprintf(stderr,"[type]%s   %s %s\n", indent.c_str(), t.c_str(), 
                n.c_str());
        RosType sub;
        if (!sub.read(t.c_str(),env,gen,nesting+1)) {
            fprintf(stderr, "[type]%s Type not complete: %s\n", 
                    indent.c_str(), 
                    row.c_str());
            ok = false;
        }
        sub.rosName = n;
        subRosType.push_back(sub);
    } while (result!=NULL);
    fprintf(stderr,"[type]%s END %s\n", indent.c_str(), path.c_str());
    fclose(fin);

    isValid = ok;
    return isValid;
}



void RosType::show() {
    if (rosName!="") {
        printf("%s:",rosName.c_str());
    }
    if (!isValid) printf("INVALID:");
    if (isPrimitive) {
        printf("%s", rosType.c_str());
    }
    if (!isPrimitive) {
        printf("(");
        for (int i=0; i<(int)subRosType.size(); i++) {
            if (i>0) printf(" ");
            subRosType[i].show();
        }
        printf(")");
    }
    if (isArray) {
        printf("[]");
    }
}



bool RosTypeCodeGenTest::beginType(const std::string& tname,
                                   RosTypeCodeGenState& state) {
    printf("Begin %s\n", tname.c_str());
    return true;
}

bool RosTypeCodeGenTest::declareField(const RosField& field) {
    printf("Field %s\n", field.rosName.c_str());
    return true;
}

bool RosTypeCodeGenTest::readField(const RosField& field) {
    printf("READ Field %s\n", field.rosName.c_str());
    return true;
}

bool RosTypeCodeGenTest::writeField(const RosField& field) {
    printf("WRITE Field %s\n", field.rosName.c_str());
    return true;
}

bool RosTypeCodeGenTest::endType() {
    printf("End\n");
    return true;
}


bool RosType::emitType(RosTypeCodeGen& gen,
                       RosTypeCodeGenState& state) {
    if (isPrimitive) return true;
    if (state.generated.find(rosType)!=state.generated.end()) {
        return true;
    }

    if (subRosType.size()>0) {
        for (int i=0; i<(int)subRosType.size(); i++) {
            if (!subRosType[i].emitType(gen,state)) return false;
        }
    }

    state.usedVariables.clear();
    state.txt = txt;
    for (int i=0; i<(int)subRosType.size(); i++) {
        state.useVariable(subRosType[i].rosName);
    }

    if (!gen.beginType(rosType,state)) return false;

    if (!gen.beginDeclare()) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.declareField(subRosType[i])) return false;
    }
    if (!gen.endDeclare()) return false;

    if (!gen.beginRead()) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.readField(subRosType[i])) return false;
    }
    if (!gen.endRead()) return false;

    if (!gen.beginWrite()) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.writeField(subRosType[i])) return false;
    }
    if (!gen.endWrite()) return false;

    if (!gen.endType()) return false;

    state.generated[rosType] = true;
    state.dependencies.push_back(rosType);
    return true;
}


std::string RosTypeSearch::findFile(const char *tname) {
    //fprintf(stderr, "[type] Looking for definition of %s\n", tname);
    string target = string(tname) + ".msg";
    for (int i=0; i<(int)target.length(); i++) {
        if (target[i]=='/') {
            target[i] = '_';
        }
    }
    struct stat dummy;
	if (stat(target.c_str(), &dummy)==0) {
        return target;
    }
    string cmd = string("rosmsg show -r ")+tname+" > " + target + " || rm -f " + target;
    fprintf(stderr,"[ros]  %s\n", cmd.c_str());
    pid_t p = ACE_OS::fork();
    if (p==0) {
        ACE_OS::execlp("sh","sh","-c",cmd.c_str(),(char*)NULL);
        exit(0);
    } else {
        ACE_OS::wait(NULL);
    }
    //printf("Ran [%s]\n", cmd.c_str());
    return target;
}


