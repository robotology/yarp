// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/String.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>

#include <fstream>

using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;

using namespace std;

#ifdef YARP2_LINUX
#define CHECK_FOR_LEAKS
#endif

#ifdef CHECK_FOR_LEAKS
// this is just for memory leak checking, and only works in linux
#include <mcheck.h>
#endif


#ifndef DOXYGEN_SHOULD_SKIP_THIS
class YarpDevMonitor : public SearchMonitor {
private:
    Property comment, fallback, present;
    Bottle order;

public:
    YarpDevMonitor() {
    }

    virtual void report(const SearchReport& report, const char *context) {
        String ctx = context;
        ConstString key = report.key;
        String prefix = "";

        // just work with device, not subdevices
        if (!(ctx.substr(0,7)=="harness")) {
            return;
        }

        // but normal nested properties are worth documenting
        if (ctx.substr(0,8)=="harness.") {
            prefix = ctx.substr(8,ctx.length());
            prefix += ".";
        }

        key = (prefix + key.c_str()).c_str();

        if (key=="device"||key=="wrapped") {
            return;
        }
        

        if (!present.check(key)) {
            present.put(key,"present");
            order.addString(key);
        }

        if (report.isComment==true) {
            comment.put(key,report.value);
            return;
        }

        if (report.isDefault==true) {
            fallback.put(key,report.value);
            return;
        }
    }

    void toDox(ostream& os) {
        os << "<table>" << endl;
        os << "<tr><td>PROPERTY</td><td>DESCRIPTION</td><td>DEFAULT</td></tr>"
           << endl;
        for (int i=0; i<order.size(); i++) {
            ConstString name = order.get(i).toString();
            ConstString desc = comment.find(name).toString();
            ConstString def = fallback.find(name).asString();
            String out = "";
            out += "<tr><td>";
            out += name.c_str();
            out += "</td><td>";
            out += desc.c_str();
            out += "</td><td>";
            out += def.c_str();
            out += "</td></tr>";
            os << out.c_str() << endl;
        }
        os << "</table>" << endl;
    }
};

String getFile(const char *fname) {
    ifstream fin(fname);
    String txt;
    if (fin.fail()) {
        return "";
    }
    while (!(fin.eof()||fin.fail())) {
        char buf[1000];
        fin.getline(buf,sizeof(buf));
        if (!fin.eof()) {
            txt += buf;
            txt += "\n";
        }
    }
    return txt;
}
#endif

int main(int argc, char *argv[]) {

#ifdef CHECK_FOR_LEAKS
    mtrace();
#endif

    int result = 0;

    Network::init();
    Network::setLocalMode(true);

    Property p;
    p.fromCommand(argc,argv);

    // check where to put description of device
    ConstString dest = "";
    dest = p.check("doc",Value("")).toString();

    ConstString fileName = p.check("file",Value("default.ini")).asString();

    if (p.check("file")) {
        p.fromConfigFile(fileName);
    }

    ConstString deviceName = p.check("device",Value("")).asString();
    
    String seek = fileName.c_str();
    ConstString exampleName = "";
    int pos = seek.rfind('/');
    if (pos==-1) {
        pos = seek.rfind('\\');
    }
    if (pos==-1) {
        pos = 0;
    } else {
        pos++;
    }
    int len = seek.find('.',pos);
    if (len==-1) {
        len = seek.length();
    } else {
        len -= pos;
    }
    exampleName = seek.substr(pos,len).c_str();
    ConstString shortFileName = seek.substr(pos,seek.length()).c_str();

    YarpDevMonitor monitor;
    p.setMonitor(&monitor,"harness");

    PolyDriver dd;
    bool ok = dd.open(p);
    result = ok?0:1;
    if (ok) {
        dd.close();
    }

    Network::fini();


    if (dest!="") {
        ofstream fout(dest.c_str());
        if (!fout.good()) {
            printf("Problem writing to %s\n", dest.c_str());
            exit(1);
        }
        fout << "/**" << endl;
        fout << " * \\ingroup dev_examples" << endl;
        fout << " *" << endl;
        fout << " * \\defgroup " 
             << exampleName.c_str() 
             << " Example for " 
             << deviceName.c_str()
             << " ("
             << exampleName.c_str() 
             << ")"
             << endl;
        fout << endl;
        fout << "Configuration file: " 
             << shortFileName.c_str()
             << endl;
        fout << "\\verbatim" << endl;
        fout << getFile(fileName).c_str();
        fout << "\\endverbatim" << endl;
        fout << endl;
        fout << "Here is a list of properties checked when starting up a device based on this configuration file.  Note that which properties are checked can depend on whether other properties are present.  In some cases properties can also vary between operating systems.  So this is just an example" << endl;
        fout << endl;
        monitor.toDox(fout);
        fout << endl;
        fout << " */" << endl;
        fout.close();
    }

    // just checking for crashes, not device creation
    return 0; //result;
}

