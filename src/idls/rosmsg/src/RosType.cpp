// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "RosType.h"
#include "md5.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <vector>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>

#ifdef YARP_PRESENT
// this code used to be compilable without yarp, I have let this rust but
// it would be straightforward to make it do so again
#  include <yarp/conf/system.h>
#endif
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

using namespace std;

vector<string> normalizedMessage(const string& line) {
    vector<string> all;
    string result;
    bool quote = false;
    bool pending = false;
    bool can_quote = true;
    for (int i=0; i<(int)line.length(); i++) {
        char ch = line[i];
        if (ch=='\"'&&can_quote) {
            quote = !quote;
            result += ch;
            pending = true;
        } else if (quote) {
            result += ch;
            pending = true;
        } else if (ch=='\r'||ch=='\t'||((ch==' '||ch=='=')&&can_quote)) {
            if (pending) {
                if (result[0]=='#') return all;
                all.push_back(result);
                pending = false;
            }
            result = "";
            if (ch=='=') {
                all.push_back("=");
                can_quote = false;
            }
        } else {
            result += ch;
            pending = true;
        }
    }
    if (pending&&result!="") {
        if (result[0]=='#'&&can_quote) return all;
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
    if (nesting>0) env.lookForService(false); // no srv nesting allowed in ros
    //printf("Checking %s\n", tname);
    clear();
    
    string base = tname;
    rosType = base;
    if (base.length()==0) return false;
    if (base[base.length()-1]==']') {
        size_t at = base.rfind('[');
        if (at==string::npos) {
            fprintf(stderr,"dodgy array? %s\n", base.c_str());
            return false;
        }
        string idx = base.substr(at+1,base.length()-at-2);
        if (idx!="") {
            yarp::os::Bottle b(idx.c_str());
            arrayLength = b.get(0).asInt();
        } else {
            arrayLength = -1;
        }
        isArray = true;
        base = base.substr(0,at);
        rosType = base;
    }

    isStruct = true;
    rosRawType = base;
    if (base[0]>='a'&&base[0]<='z'&&base.find("/")==string::npos&&base.find(".")==string::npos) {
        isStruct = false;
        if (base=="time"||base=="duration") {
            if (gen.hasNativeTimeClass()) {
                isPrimitive = true;
            } else {
                rosType = (base=="time")?"TickTime":"TickDuration";
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
    if (rosType.find(".")!=string::npos) {
        rosType = rosType.substr(0,rosType.rfind("."));
    }

    bool ok = true;
    string path = env.findFile(base.c_str());
    rosPath = path;
 
    FILE *fin = fopen((env.getTargetDirectory() + "/" + path).c_str(),"r");
    if (!fin) {
        fin = fopen(path.c_str(),"r");
    }
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open %s\n", path.c_str());
        exit(1);
    }

    fprintf(stderr,"[type]%s BEGIN %s\n", indent.c_str(), path.c_str());
    char *result = NULL;
    txt = "";

    RosType *cursor = this;
    do {
        char buf[2048];
        result = fgets(buf,sizeof(buf),fin);
        if (result==NULL) break;
        txt += "//   ";
        txt += result;
        int len = (int)strlen(result);
        for (int i=0; i<len; i++) {
            if (result[i]=='\n') {
                result[i] = '\0';
                break;
            }
        }
        string row = result;
        vector<string> msg = normalizedMessage(row);
        if (msg.size()==0) { continue; }
        if (msg[0] == "---") {
            printf("--- reply ---\n");
            cursor->isValid = ok;
            ok = true;
            cursor->reply = new RosType();
            cursor = cursor->reply;
            cursor->rosType = rosType + "Reply";
            continue;
        }
        bool have_const = false;
        string const_txt = "";
        if (msg.size()>2) {
            if (msg[2]=="=") {
                have_const = true;
                //printf("Not worrying about: %s\n", row.c_str());
                //continue;
                const_txt = msg[3];
            }
        }
        if (msg.size()!=2 && !have_const) {
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
        fprintf(stderr,"[type]%s   %s %s", indent.c_str(), t.c_str(), 
                n.c_str());
        if (const_txt!="") {
            fprintf(stderr," = %s", const_txt.c_str());
        }
        fprintf(stderr,"\n");
        RosType sub;
        if (!sub.read(t.c_str(),env,gen,nesting+1)) {
            fprintf(stderr, "[type]%s Type not complete: %s\n", 
                    indent.c_str(), 
                    row.c_str());
            ok = false;
        }
        sub.rosName = n;
        sub.initializer = const_txt;
        cursor->subRosType.push_back(sub);
    } while (result!=NULL);
    fprintf(stderr,"[type]%s END %s\n", indent.c_str(), path.c_str());
    fclose(fin);

    cursor->isValid = ok;
    return isValid;
}

bool RosType::cache(const char *tname, RosTypeSearch& env, 
                    RosTypeCodeGen& gen) {
    string rosType = tname;
    if (rosType.find(".")==string::npos) {
        return false;
    }
    rosType = rosType.substr(0,rosType.rfind("."));
    FILE *fin = fopen((env.getTargetDirectory() + "/" + tname).c_str(),"r");
    FILE *fout = fopen((env.getTargetDirectory() + "/" + rosType).c_str(),"w");
    if (!fin) {
        fin = fopen(tname,"r");
    }
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open %s\n", tname);
        exit(1);
    }
    if (!fout) {
        fprintf(stderr, "[type] FAILED to open %s\n", rosType.c_str());
        exit(1);
    }

    do {
        char buf[2048];
        char *result = fgets(buf,sizeof(buf),fin);
        if (result==NULL) break;
        fputs(result,fout);
    } while (true);
    fclose(fout);
    fclose(fin);
    return true;
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



bool RosType::emitType(RosTypeCodeGen& gen,
                       RosTypeCodeGenState& state) {
    if (isPrimitive) return true;
    if (state.generated.find(rosType)!=state.generated.end()) {
        return true;
    }

    if (subRosType.size()>0) {
        for (int i=0; i<(int)subRosType.size(); i++) {
            RosType& e = subRosType[i];
            if (!e.emitType(gen,state)) return false;

            if (e.isConst()) {
                string add = e.rosType + " " + e.rosName + "=" + e.initializer + "\n";
                checksum_const_text.push_back(add);
            } else {
                string add = "";
                if (!e.isStruct) {
                    add += e.rosRawType;
                } else {
                    add += e.checksum;
                }
                if (e.isArray && !e.isStruct) {
                    add += "[";
                    if (e.arrayLength!=-1) {
                        add += yarp::os::Bottle::toString(e.arrayLength);
                    }
                    add += "]";
                }
                add += " ";
                add += e.rosName;
                add += "\n";
                checksum_var_text.push_back(add);
            }
        }
    }

    string sum = "";
    for (list<string>::iterator it=checksum_const_text.begin();
         it!=checksum_const_text.end(); it++) {
        sum += *it;
    }
    for (list<string>::iterator it=checksum_var_text.begin();
         it!=checksum_var_text.end(); it++) {
        sum += *it;
    }
    sum = sum.substr(0,sum.length()-1);
    md5_state_t cstate;
    md5_byte_t digest[16];
    md5_init(&cstate);
    md5_append(&cstate, (const md5_byte_t *)sum.c_str(), sum.length());
    md5_finish(&cstate, digest);
    char hex_output[16*2 + 1];
    for (int di = 0; di<16; di++) {
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
    }
    checksum = (char *)hex_output;

    if (reply!=NULL) {
        if (!reply->emitType(gen,state)) return false;
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

    if (!gen.beginConstruct()) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.constructField(subRosType[i])) return false;
    }
    if (!gen.endConstruct()) return false;

    if (!gen.beginRead(true,(int)subRosType.size())) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.readField(true,subRosType[i])) return false;
    }
    if (!gen.endRead(true)) return false;

    if (!gen.beginRead(false,(int)subRosType.size())) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.readField(false,subRosType[i])) return false;
    }
    if (!gen.endRead(false)) return false;

    if (!gen.beginWrite(true,(int)subRosType.size())) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.writeField(true,subRosType[i])) return false;
    }
    if (!gen.endWrite(true)) return false;

    if (!gen.beginWrite(false,(int)subRosType.size())) return false;
    for (int i=0; i<(int)subRosType.size(); i++) {
        if (!gen.writeField(false,subRosType[i])) return false;
    }
    if (!gen.endWrite(false)) return false;

    if (!gen.endType(rosType,*this)) return false;

    state.generated[rosType] = true;
    state.dependencies.push_back(rosType);
    state.dependenciesAsPaths.push_back((rosPath=="")?rosType:rosPath);

    return true;
}


static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::string RosTypeSearch::readFile(const char *fname) {
    char buf[25600];
    FILE *fin = fopen(fname,"r");
    if (fin==NULL) return "";
    std::string result = "";
    while(fgets(buf, sizeof(buf)-1, fin) != NULL) {
        result += buf;
    }
    fclose(fin);
    fin = NULL;
    return result;
}


static bool checkWeb(const char *tname,
                     bool find_service,
                     const string& target_full) {
    bool success = false;
    string name = tname;
    size_t idx = name.find("/");
    if (idx!=string::npos) {
        string package = name.substr(0,idx);
        string typ = name.substr(idx+1,name.length());
        string url = "http://docs.ros.org:80/api/";
        url += package;
        if (find_service) {
            url += "/html/srv/";
        } else {
            url += "/html/msg/";
        }
        url += typ;
        url += ".html";
        fprintf(stderr, "Trying the web: %s\n", url.c_str());
        yarp::os::NetworkBase yarp;
        yarp::os::Port port;
        port.openFake("base");
        port.addOutput(url);
        yarp::os::Bottle cmd, reply;
        cmd.addString("1");
        port.write(cmd,reply);
        string txt = reply.get(0).asString() + "\n";
        printf("GOT %s for %s\n", txt.c_str(), url.c_str());
        vector<string> lines;
        split(txt,'\n',lines);
        for (size_t i=0; i<lines.size(); i++) {
            std::string line = lines[i];
            if (line == "<h2>Compact Message Definition</h2>" && i<lines.size()-2) {
                std::string def = lines[i+2];
                std::string def2;
                std::string tag;
                bool tagging = false;
                for (size_t j=0; j<def.length(); j++) {
                    char ch = def[j];
                    if (tagging||ch=='<') {
                        tagging = true;
                        tag += ch;
                        if (ch=='>') {
                            tagging = false;
                            if (tag=="<br />") {
                                def2 += "\n";
                            }
                            if (tag=="<hr />") {
                                def2 += "---\n";
                            }
                            tag = "";
                        }
                    } else {
                        def2 += ch;
                    }
                }
                FILE *fout = fopen(target_full.c_str(),"w");
                if (fout) {
                    fprintf(fout,"%s",def2.c_str());
                    fclose(fout);
                    success = true;
                }
            }
        }
    }
    return success;
}

std::string RosTypeSearch::findFile(const char *tname) {
    struct stat dummy;
	if (stat(tname, &dummy)==0) {
        return tname;
    }
    string target = string(tname);
    if (target.find(".")!=string::npos) {
        return tname;
    }
    for (int i=0; i<(int)target.length(); i++) {
        if (target[i]=='/') {
            target[i] = '_';
        }
    }
    string target_full = target_dir + "/" + target;
	if (stat(target_full.c_str(), &dummy)==0) {
        return target;
    }

    if (std::string(tname)=="Header") {
        // support Header natively, for the sake of tests
        FILE *fout = fopen(target_full.c_str(),"w");
        if (fout) {
            fprintf(fout,"[roslib/Header]\n");
            fprintf(fout,"uint32 seq\n");
            fprintf(fout,"time stamp\n");
            fprintf(fout,"string frame_id\n");
        }
        fclose(fout);
    } else {
        string cmd = string(find_service?"rossrv":"rosmsg") + " show -r "+tname+" > " + target_full + " || rm -f " + target_full;
        fprintf(stderr,"[ros]  %s\n", cmd.c_str());
        pid_t p = ACE_OS::fork();
        if (p==0) {
#ifdef __linux__
            // This was ACE_OS::execlp, but that fails
            ::execlp("sh","sh","-c",cmd.c_str(),(char *)NULL);
#else
            ACE_OS::execlp("sh","sh","-c",cmd.c_str(),(char *)NULL);
#endif
            exit(0);
        } else {
            ACE_OS::wait(NULL);
        }
    } 

    bool success = true;

    FILE *fin = fopen(target_full.c_str(),"r");
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open %s\n", target_full.c_str());
        success = false;
    } else {
        char buf[10];
        char *result = fgets(buf,sizeof(buf),fin);
        fclose(fin);
        if (result==NULL) {
            fprintf(stderr, "[type] File is blank: %s\n", target_full.c_str());
            ACE_OS::unlink(target_full.c_str());
            success = false;
        }
    }

    if (allow_web && !success) {
        success = checkWeb(tname,find_service,target_full);
        if (!success) {
            if (!find_service) {
                success = checkWeb(tname,true,target_full);
            }
        }
    }

    if (!success) {
        target = "";
        if (abort_on_error) exit(1);
    }

    return target;
}


