/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RosType.h"
#include "md5.h"

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Os.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformSysWait.h>
#include <yarp/os/impl/PlatformUnistd.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


std::vector<std::string> normalizedMessage(const std::string& line) {
    std::vector<std::string> all;
    std::string result;
    bool quote = false;
    bool pending = false;
    bool can_quote = true;
    for (char ch : line) {
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
                all.emplace_back("=");
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
    }
    return all;
}

bool RosType::read(const char *tname, RosTypeSearch& env, RosTypeCodeGen& gen,
                   int nesting) {
    std::string indent;
    for (int i=0; i<nesting; i++) {
        indent += "  ";
    }
    if (nesting>0) {
        env.lookForService(false); // no srv nesting allowed in ros
    }

    if (verbose) {
        printf("[type]%s Checking %s\n", indent.c_str(), tname);
    }
    clear();

    std::string base = tname;
    rosType = base;
    if (base.length()==0) {
        return false;
    }
    if (base[base.length()-1]==']') {
        size_t at = base.rfind('[');
        if (at==std::string::npos) {
            fprintf(stderr,"dodgy array? %s\n", base.c_str());
            return false;
        }
        std::string idx = base.substr(at+1,base.length()-at-2);
        if (idx!="") {
            yarp::os::Bottle b(idx);
            arrayLength = b.get(0).asInt32();
        } else {
            arrayLength = -1;
        }
        isArray = true;
        base = base.substr(0,at);
        rosType = base;
    }

    isStruct = true;
    rosRawType = base;
    if (base[0]>='a'&&base[0]<='z'&&base.find('/')==std::string::npos&&base.find('.')==std::string::npos) {
        isStruct = false;
        isRosPrimitive = true;
        if (base=="time"||base=="duration") {
            if (gen.hasNativeTimeClass()) {
                isPrimitive = true;
            } else {
                rosType = (base=="time")?"TickTime":"TickDuration";
                isStruct = true;
                isPrimitive = false;
                RosType t1;
                t1.rosType = "uint32";
                t1.rosRawType = "uint32";
                t1.rosName = "sec";
                t1.isPrimitive = true;
                t1.isRosPrimitive = true;
                t1.isValid = true;
                t1.verbose = verbose;
                subRosType.push_back(t1);
                RosType t2;
                t2.rosType = "uint32";
                t2.rosRawType = "uint32";
                t2.rosName = "nsec";
                t2.isPrimitive = true;
                t2.isRosPrimitive = true;
                t2.isValid = true;
                t2.verbose = verbose;
                subRosType.push_back(t2);
            }
        } else {
            isPrimitive = true;
            isRosPrimitive = true;
        }
        isValid = true;
        return true;
    }
    if (rosType.find('.')!=std::string::npos) {
        rosType = rosType.substr(0,rosType.rfind('.'));
    }

    if (isStruct) {
        size_t at = rosType.rfind('/');
        if (at != std::string::npos) {
            std::string temp = rosType.substr(0, at);
            at = temp.rfind('/');
            if (at != std::string::npos) {
                package = temp.substr(at+1);
            } else {
                package = temp;
            }
            if (package == "srv" || package == "msg")
            {
                at = temp.rfind('/');
                if (at != std::string::npos) {
                    temp = temp.substr(0, at);
                    at = temp.rfind('/');
                    if (at != std::string::npos) {
                        package = temp.substr(at+1);
                    } else {
                        package = temp;
                    }
                }
            }
        } else if (rosType == "Header" || rosType == "std_msgs/Header") {
            rosType = "std_msgs/Header";
            base = "std_msgs/Header";
        } else if (package != "" && package != ".") {
            rosType = package + "/" + rosType;
            base = package + "/" + base;
        }
    }

    bool ok = true;
    char dot = '.';
    int count = std::count(base.begin(), base.end(), dot);
    std::string str3(base);
    if (count == 2){
        int pos1 = base.find(dot,0);
        int pos2 = base.find(dot,pos1);
        str3 = base.substr(0,pos2);
    }
    std::string path = env.findFile(str3.c_str());
    rosPath = path;

    FILE *fin = fopen((env.getTargetDirectory() + "/" + path).c_str(),"r");
    if (!fin) {
        fin = fopen(path.c_str(),"r");
    }
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open %s\n", path.c_str());
        std::exit(1);
    }

    if (verbose) {
        fprintf(stderr,"[type]%s BEGIN %s\n", indent.c_str(), path.c_str());
    }
    char *result = nullptr;
    txt = "";
    source = "";

    RosType *cursor = this;
    char buf[2048];
    do {
        result = fgets(buf,sizeof(buf),fin);
        if (result==nullptr) break;
        txt += "//   ";
        txt += result;
        source += result;
        int len = (int)strlen(result);
        for (int i=0; i<len; i++) {
            if (result[i]=='\n') {
                result[i] = '\0';
                break;
            }
        }
        std::string row = result;
        std::vector<std::string> msg = normalizedMessage(row);
        if (msg.size()==0) { continue; }
        if (msg[0] == "---") {
            if (verbose) {
                printf("--- reply ---\n");
            }
            cursor->isValid = ok;
            ok = true;
            cursor->reply = new RosType();
            cursor->reply->verbose = verbose;
            cursor = cursor->reply;
            cursor->rosType = rosType + "Reply";
            continue;
        }
        bool have_const = false;
        std::string const_txt;
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
                    if (verbose) {
                        fprintf(stderr,"[type] skip %s\n", row.c_str());
                    }
                    ok = false;
                }
            }
            continue;
        }
        std::string t = msg[0];
        std::string n = msg[1];
        if (verbose) {
            fprintf(stderr,"[type]%s   %s %s", indent.c_str(), t.c_str(), n.c_str());
            if (const_txt!="") {
                fprintf(stderr," = %s", const_txt.c_str());
            }
            fprintf(stderr,"\n");
        }
        RosType sub;
        sub.verbose = verbose;
        sub.package = package;
        if (!sub.read(t.c_str(), env, gen, nesting+1)) {
            fprintf(stderr, "[type]%s Type not complete: %s\n",
                    indent.c_str(),
                    row.c_str());
            ok = false;
        }

        sub.rosName = n;
        const_txt.erase(0,const_txt.find_first_not_of(" \t"));
        if (const_txt.find_first_of(" \t#")!=std::string::npos) {
            const_txt = const_txt.substr(0,const_txt.find_first_of(" \t#"));
        }
        sub.initializer = const_txt;
        cursor->subRosType.push_back(sub);
    } while (result!=nullptr);
    if (verbose) {
        fprintf(stderr,"[type]%s END %s\n", indent.c_str(), path.c_str());
    }
    fclose(fin);

    if (rosType == "Header" || rosType == "std_msgs/Header") {
        std::string preamble = "[std_msgs/Header]:";
        if (source.find(preamble)==0) {
            source = source.substr(preamble.length()+1,source.length());
        }
    }

    isValid = ok;
    if (!isValid) {
        fprintf(stderr, "[type]%s Check failed: %s\n", indent.c_str(), tname);
    }

    return isValid;
}

bool RosType::cache(const char *tname, RosTypeSearch& env,
                    RosTypeCodeGen& gen) {
    std::string rosType = tname;
    size_t at = rosType.rfind('.');
    if (at == std::string::npos) {
        return false;
    }
    std::string ext = rosType.substr(at+1);
    rosType = rosType.substr(0,at);
    std::string pkg;
    at = rosType.rfind('/');
    if (at != std::string::npos && at > 0) {
        size_t pkg_at = rosType.rfind('/', at-1);
        pkg = rosType.substr(pkg_at+1, at-pkg_at-1);
        if (pkg == "srv" || pkg == "msg") {
            pkg_at = rosType.rfind('/', pkg_at-1);
            pkg = rosType.substr(pkg_at+1, at-pkg_at-5); // -1 -("/msg" or "/srv") = -5
        }
    }
    rosType = rosType.substr(rosType.rfind('/')+1);
    std::string outfile = rosType + "." + ext;
    if (!pkg.empty()) {
        outfile = pkg + "/" + outfile;
    }

    // If input and output are the same there is nothing to do
    if (tname == outfile) {
        return true;
    }

    FILE *fin = fopen((env.getTargetDirectory() + "/" + tname).c_str(),"r");
    yarp::os::mkdir_p((env.getTargetDirectory() + "/" + pkg).c_str(),1);
    FILE *fout = fopen((env.getTargetDirectory() + "/" + outfile).c_str(),"w");
    if (!fin) {
        fin = fopen(tname,"r");
    }
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open input file \"%s\"\n", (env.getTargetDirectory() + "/" + tname).c_str());
        std::exit(1);
    }
    if (!fout) {
        fprintf(stderr, "[type] FAILED to open output file \"%s\"\n", (env.getTargetDirectory() + "/" + outfile).c_str());
        std::exit(1);
    }

    do {
        char buf[2048];
        char *result = fgets(buf,sizeof(buf),fin);
        if (result==nullptr) break;
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
        for (size_t i=0; i<subRosType.size(); i++) {
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
    if (isPrimitive) {
        return true;
    }
    if (state.generated.find(rosType)!=state.generated.end()) {
        checksum = state.generated[rosType]->checksum;
        return true;
    }

    for (RosType& e : subRosType) {
        if (!e.emitType(gen,state)) {
            return false;
        }

        if (e.isConst()) {
            std::string add = e.rosType + " " + e.rosName + "=" + e.initializer + "\n";
            checksum_const_text.push_back(add);
        } else {
            std::string add;
            if (e.isRosPrimitive) {
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

    std::string sum;
    for (const auto& txt : checksum_const_text) {
        sum += txt;
    }
    for (const auto& txt : checksum_var_text) {
        sum += txt;
    }
    //printf("SUM [%s] [%s]\n", rosType.c_str(), sum.c_str());
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
    //printf("CHECKSUM [%s] [%s]\n", rosType.c_str(), checksum.c_str());

    if (reply!=nullptr) {
        if (!reply->emitType(gen,state)) {
            return false;
        }
    }


    state.usedVariables.clear();
    state.txt = txt;
    for (auto& i : subRosType) {
        state.useVariable(i.rosName);
    }

    for (auto& i : subRosType) {
        if (i.isStruct && std::find(state.dependencies[rosType].begin(), state.dependencies[rosType].end(), i.rosType) == state.dependencies[rosType].end()) {
            state.dependencies[rosType].push_back(i.rosType);
            state.dependenciesAsPaths[rosType].push_back((i.rosPath=="")?i.rosType:i.rosPath);
        }
    }

    if (!gen.beginType(rosType,state)) return false;

    if (!gen.beginDeclare()) return false;
    for (const auto& i : subRosType) {
        if (!gen.declareField(i)) return false;
    }
    if (!gen.endDeclare()) return false;

    if (!gen.beginConstruct()) return false;
    bool isFirst = true;
    for (const auto& i : subRosType) {
        if (!gen.initField(i, isFirst)) return false;
    }
    if (!gen.endInitConstruct()) return false;
    for (const auto& i : subRosType) {
        if (!gen.constructField(i)) return false;
    }
    if (!gen.endConstruct()) return false;

    if (!gen.beginClear()) return false;
    for (const auto& i : subRosType) {
        if (!gen.clearField(i)) return false;
    }
    if (!gen.endClear()) return false;

    if (!gen.beginRead(true,(int)subRosType.size())) return false;
    for (const auto& i : subRosType) {
        if (!gen.readField(true,i)) return false;
    }
    if (!gen.endRead(true)) return false;

    if (!gen.beginRead(false,(int)subRosType.size())) return false;
    for (const auto& i : subRosType) {
        if (!gen.readField(false,i)) return false;
    }
    if (!gen.endRead(false)) return false;

    if (!gen.beginWrite(true,(int)subRosType.size())) return false;
    for (const auto& i : subRosType) {
        if (!gen.writeField(true,i)) return false;
    }
    if (!gen.endWrite(true)) return false;

    if (!gen.beginWrite(false,(int)subRosType.size())) return false;
    for (const auto& i : subRosType) {
        if (!gen.writeField(false,i)) return false;
    }
    if (!gen.endWrite(false)) return false;

    if (!gen.endType(rosType,*this)) return false;

    state.generated[rosType] = this;

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
    if (fin==nullptr) return {};
    std::string result;
    while(fgets(buf, sizeof(buf)-1, fin) != nullptr) {
        result += buf;
    }
    fclose(fin);
    fin = nullptr;
    return result;
}

bool RosTypeSearch::fetchFromRos(const std::string& target_file,
                                 const std::string& type_name,
                                 bool find_service) {
    std::string cmd = std::string(find_service?"rossrv":"rosmsg") + " show -r "+type_name+" | install -D /dev/stdin " + target_file + " || rm -f " + type_name;
    if (verbose) {
        fprintf(stderr,"[ros]  %s\n", cmd.c_str());
    }

#if defined(YARP_HAS_ACE)
    using ACE_OS::fork;
# if defined(__linux__)
    // This was ACE_OS::execlp, but that fails
    using ::execlp;
# else
    using ACE_OS::execlp;
# endif
#elif defined(__unix__)
    using ::fork;
    using ::execlp;
#endif

    pid_t p = fork();
    if (p==0) {
        execlp("sh", "sh", "-c", cmd.c_str(), (char *)nullptr);
        std::exit(0);
    } else {
        yarp::os::impl::wait(nullptr);
    }

    bool success = true;

    FILE *fin = fopen(target_file.c_str(),"r");
    if (!fin) {
        fprintf(stderr, "[type] FAILED to open %s\n", target_file.c_str());
        success = false;
    } else {
        char buf[10];
        char *result = fgets(buf,sizeof(buf),fin);
        fclose(fin);
        if (result==nullptr) {
            fprintf(stderr, "[type] File is blank: %s\n", target_file.c_str());
            yarp::os::impl::unlink(target_file.c_str());
            success = false;
        }
    }

    return success;
}

bool RosTypeSearch::fetchFromWeb(const std::string& target_file,
                                 const std::string& type_name,
                                 bool find_service) {
    bool success = false;
    size_t idx = type_name.find('/');
    if (idx!=std::string::npos) {
        std::string package = type_name.substr(0,idx);
        std::string typ = type_name.substr(idx+1,type_name.length());
        std::string url = "http://docs.ros.org:80/api/";
        url += package;
        if (find_service) {
            url += "/html/srv/";
        } else {
            url += "/html/msg/";
        }
        url += typ;
        url += ".html";
        if (verbose) {
            fprintf(stderr, "Trying the web: %s\n", url.c_str());
        }
        yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
        yarp::os::Port port;
        port.openFake("base");
        port.addOutput(url);
        yarp::os::Bottle cmd, reply;
        cmd.addString("1");
        port.write(cmd,reply);
        std::string txt = reply.get(0).asString() + "\n";
        if (verbose) {
            printf("GOT %s for %s\n", txt.c_str(), url.c_str());
        }
        std::vector<std::string> lines;
        split(txt,'\n',lines);
        for (size_t i=0; i<lines.size(); i++) {
            std::string line = lines[i];
            if (line == "<h2>Compact Message Definition</h2>" && i<lines.size()-2) {
                std::string def = lines[i+2];
                std::string def2;
                std::string tag;
                bool tagging = false;
                for (char ch : def) {
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
                yarp::os::mkdir_p(target_file.c_str(),1);
                FILE *fout = fopen(target_file.c_str(),"w");
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
    std::string target = std::string(tname);

    // If this is a path to a file, return the path, if the file exists.
    if (stat(tname, &dummy) == 0)
    {
        if (source_dir.empty() && package_name.empty()) {
            size_t at = target.rfind('/');
            if (at != std::string::npos) {
                source_dir = target.substr(0, at);
                at = source_dir.rfind('/');
                if (at != std::string::npos) {
                    package_name = source_dir.substr(at+1);
                    source_dir = source_dir.substr(0, at);
                }
                if (package_name == "srv" || package_name == "msg")
                {
                    size_t at = source_dir.rfind('/');
                    if (at != std::string::npos) {
                        package_name = source_dir.substr(at+1);
                        source_dir = source_dir.substr(0, at);
                    }
                }
            } else {
                source_dir = ".";
            }
        }
        return target;
    }

    std::string target2;
    if (!source_dir.empty()) {
        size_t at = target.rfind('/');
        if (at != std::string::npos) {
            target2 = target.substr(0, at) + (find_service? "/srv/" : "/msg/") + target.substr(at+1);
        }

        std::string source_full;

        // Search in source directory
        source_full = source_dir + "/" + target + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s... (source directory: %s)\n", source_full.c_str(), source_dir.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search in source directory (adding src/msg to path)
        source_full = source_dir + "/" + target2 + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s... (source directory: %s)\n", source_full.c_str(), source_dir.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search in parent source directory
        source_full = source_dir + "/../" + target + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s... (parent source directory: %s)\n", source_full.c_str(), source_dir.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search in parent source directory (adding src/msg to path)
        source_full = source_dir + "/../" + target2 + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s... (parent source directory: %s)\n", source_full.c_str(), source_dir.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search for current package in source directory
        source_full = source_dir + "/" + package_name + "/" + target + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s... (package name: %s)\n", source_full.c_str(), package_name.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search for current package in source directory (adding src/msg to path)
        source_full = source_dir + "/" + package_name + "/" + target2 + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s... (package name: %s)\n", source_full.c_str(), package_name.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search for std_msgs package in source directory
        source_full = source_dir + "/std_msgs/" + target + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s...\n", source_full.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search for std_msgs package in source directory (adding src/msg to path)
        source_full = source_dir + "/std_msgs/" + target2 + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s...\n", source_full.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search for common_msgs package in parent source directory
        source_full = source_dir + "/../common_msgs/" + target + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s...\n", source_full.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search for common_msgs package in parent source directory (adding src/msg to path)
        source_full = source_dir + "/../common_msgs/" + target2 + (find_service? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s...\n", source_full.c_str());
        }
        if (stat(source_full.c_str(), &dummy)==0) {
            return source_full;
        }

        // Search in current directory
        source_full = "./" + target + (find_service ? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s...\n", source_full.c_str());
        }
        if (stat(source_full.c_str(), &dummy) == 0) {
            return source_full;
        }

        // Search in current directory (adding src/msg to path)
        source_full = "./" + target2 + (find_service ? ".srv" : ".msg");
        if (verbose)
        {
            printf("searching definition: %s...\n", source_full.c_str());
        }
        if (stat(source_full.c_str(), &dummy) == 0) {
            return source_full;
        }
    }
    else
    {
        if (verbose)
        {
            printf("source dir is empty");
        }
    }

    // Search in target directory (already fetched from ROS/web, no need to
    // fetch it again)
    std::string target_full = target_dir + "/" + target + (find_service? ".srv" : ".msg");
    if (verbose)
    {
        printf("searching definition in target directory %s...\n", target_full.c_str());
    }
    if (stat(target_full.c_str(), &dummy)==0) {
        return target_full;
    }

    // If not in sources, try to fetch it from ROS
    if (allow_ros)
    {
        if (verbose)
        {
            printf("searching definition from ros...\n");
        }
        bool success = fetchFromRos(target_full, tname, find_service);
        if (success) {
            return target_full;
        }
    }

    // try to fetch it from the web
    if (allow_web)
    {
        if (verbose)
        {
            printf("searching definition from the web...\n");
        }
        bool success = fetchFromWeb(target_full, tname, find_service);
        if (success) {
            return target_full;
        }
    }

    // support Header natively, for the sake of tests
    if (target == "std_msgs/Header")
    {
        if (verbose)
        {
            printf("using internal std_msgs/Header support\n");
        }
        yarp::os::mkdir_p(target_full.c_str(),1);
        FILE *fout = fopen(target_full.c_str(),"w");
        if (fout) {
            fprintf(fout,"uint32 seq\n");
            fprintf(fout,"time stamp\n");
            fprintf(fout,"string frame_id\n");
            fclose(fout);
        }
        return target_full;
    }

    // File not found. abort if needed
    if (abort_on_error) {
        fprintf(stderr, "[type] %s not found. Aborting\n", tname);
        std::exit(1);
    } else {
        fprintf(stderr, "[type] %s not found. Continuing\n", tname);
    }

    return {};
}
