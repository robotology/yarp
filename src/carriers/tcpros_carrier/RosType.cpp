// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <string.h>

#include "RosType.h"

#include <vector>

#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
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


bool RosType::read(const char *tname, RosTypeSearch& env, RosTypeCodeGen& gen) {
    printf("Checking %s\n", tname);
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
    if (!fin) return false;

    printf("BEGIN SCAN %s\n", path.c_str());
    char *result = NULL;
    do {
        char buf[2048];
        result = fgets(buf,sizeof(buf),fin);
        if (result==NULL) break;
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
            printf("Line skipped: %s\n", row.c_str());
            ok = false;
            continue;
        }
        string t = msg[0];
        string n = msg[1];
        printf("TYPE %s NAME %s\n", t.c_str(), n.c_str());
        RosType sub;
        if (!sub.read(t.c_str(),env,gen)) {
            printf("Type not complete: %s\n", row.c_str());
            ok = false;
        }
        sub.rosName = n;
        subRosType.push_back(sub);
    } while (result!=NULL);
    printf("END SCAN %s\n", path.c_str());

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
    printf("Looking for %s\n", tname);
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
    string cmd = string("rosmsg show -r ")+tname+" > " + target;
    printf("About to run [%s]\n", cmd.c_str());
    pid_t p = fork();
    if (p==0) {
        execlp("sh","sh","-c",cmd.c_str(),
               (char*)NULL);
        exit(0);
    } else {
        wait(NULL);
    }
    printf("Ran [%s]\n", cmd.c_str());
    return target;
}


