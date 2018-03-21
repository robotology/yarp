/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2007 Giacomo Spigler
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPUTILS_H
#define YARPUTILS_H

//#define FUSE_USE_VERSION 26

#include <fuse/fuse.h>
//#include <fuse/fuse_lowlevel.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
//#include <fcntl.h>

#include <yarp/os/all.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>

#include <string>
#include <csignal>

#include <ace/Containers_T.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;



/*

We try to map from filesystem paths to port names.

The "directory" is the port name, and the "file" part is
an aspect of the port - properties, or read/write pipes.

 */
class YPath {
private:
    string path, head, tail;
    bool dirty;
    bool _isStem, _isPort, _isAct;
    bool _isSymLink;
    ConstString link;

    // New version of check method.  Relies on very up-to-date yarp server.
    // Deals better with non existent directories/files.
    void check() {
        if (dirty) {
            // limit for now: no "rw" directories
            bool leafLike = (tail=="rw");

            NameConfig nc;
            ConstString name = nc.getNamespace();
            Bottle msg, reply;
            msg.addString("bot");
            msg.addString("list");
            msg.addString(leafLike?(head.c_str()):path.c_str());
            Network::write(name.c_str(),
                           msg,
                           reply);
            printf("Check: got %s\n", reply.toString().c_str());
            _isAct = false;
            _isStem = false;
            _isPort = false;
            _isSymLink = false;
            link = "";
            if (leafLike) {
                if (reply.size()>1) {
                    _isAct = true;
                }
            } else {
                if (reply.size()>1) {
                    _isStem = true;
                    if (reply.size()==2) {
                        Property p;
                        p.fromString(reply.get(1).toString());
                        //printf("Property p: %s\n", p.toString().c_str());
                        if (p.check("name",Value("[none]")).asString()==
                            path.c_str()) {
                            _isPort = true;
                        }
                        if (p.check("carrier",Value("[none]")).asString()==
                            "symlink") {
                            _isSymLink = true;
                        }
                    }
                }
            }
            dirty = false;
        }
    }

public:
    YPath(const char *path) {
        set(path);
    }

    void set(const char *path) {
        this->path = path;
        head = path;
        tail = path;
        size_t at = tail.rfind("/");
        if (at!=tail.npos) {
            tail = tail.substr(at+1,tail.length());
            head = head.substr(0,at);
        }
        printf("PATH [%s] [%s] [%s]\n", head.c_str(), tail.c_str(),
               this->path.c_str());
        dirty = true;
    }

    string getHead() {
        return head;
    }

    string getTail() {
        return tail;
    }

    bool isStem() {
        check();
        return _isStem;
    }

    bool isPort() {
        check();
        return _isPort;
    }

    bool isAct() {
        check();
        return _isAct;
    }

    bool isSymLink() {
        check();
        return _isSymLink;
    }

    ConstString getLink() {
        if (link=="") {
            Value * v = Network::getProperty(path.c_str(),"link");
            if (v!=NULL) {
                link = v->asString();
                delete v;
            }
        }
        return link;
    }
};



class YHandle {
private:
    YPath ypath;
    BufferedPort<Bottle> port;
    bool writing, reading, opened;
public:
    YHandle(const char *path) : ypath(path) {
        writing = false;
        reading = false;
        opened = false;
    }

    string getHead() {
        return ypath.getHead();
    }

    string getTail() {
        return ypath.getTail();
    }

    bool isStem() {
        return ypath.isStem();
    }

    bool isPort() {
        return ypath.isPort();
    }

    bool isAct() {
        return ypath.isAct();
    }

    bool isSymLink() {
        return ypath.isSymLink();
    }

    ConstString getLink() {
        return ypath.getLink();
    }

    int open() {
        if (!opened) {
            port.setStrict(true);
            port.open("...");
            opened = true;
        }
        return 0;
    }

    int write(const char *buf, size_t size, off_t offset) {
        open();
        if (!writing) {
            Network::connect(port.getName(),getHead().c_str());
            writing = true;
        }
        Bottle& bot = port.prepare();
        bot.clear();
        string src(buf,size);
        bot.fromString(src.c_str());
        port.write(true);
        return size;
    }

    int read(char *buf, size_t size, off_t offset) {

        open();
        if (!reading) {
            Network::connect(getHead().c_str(),port.getName());
            reading = true;
        }

        Bottle *bot = port.read();
        if (bot==NULL) {
            return 0;
        }
        string str = bot->toString().c_str();
        str = str + "\n";

        const char *yarp_str = str.c_str();
        printf(">>> Got %s\n", str.c_str());

        offset = 0;
        size_t len = strlen(yarp_str);
        if (offset < len) {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, yarp_str + offset, size);
        } else
            size = 0;

        return size;
    }
};

#define YHANDLE(x) ((YHandle*)((x)->fh))

#endif
