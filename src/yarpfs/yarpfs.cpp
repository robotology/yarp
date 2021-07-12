/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2007 Giacomo Spigler
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define FUSE_USE_VERSION 26

#include <fuse/fuse.h>
//#include <fuse/fuse_lowlevel.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
//#include <fcntl.h>

#include <yarp/os/all.h>
#include <yarp/os/impl/NameConfig.h>

#include <string>
#include <csignal>

#include <set>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace std;


#include "yarputils.h"
#include "yarpfns.h"



int yarp_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    YPath ypath(path);

    memset(stbuf, 0, sizeof(struct stat));
    printf("Checking attr // port %d, stem %d, act %d, sym %d\n",
           ypath.isPort(),
           ypath.isStem(),
           ypath.isAct(),
           ypath.isSymLink());
    if (ypath.isSymLink()) {
        stbuf->st_mode = S_IFLNK | 0755;
        stbuf->st_nlink = 1;
        stbuf->st_size = ypath.getLink().length()+1;
        printf("Link size %d\n", stbuf->st_size);
    } else if (ypath.isStem()) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (ypath.isAct()) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 0; //strlen(yarp_str);
    }
    else {
        res = -ENOENT;
    }

    return res;
}

int yarp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi)
{
    (void /* unused */) offset;
    (void /* unused */) fi;

    printf(">>>>>>>>>>>READING DIR\n");


    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    YPath ypath(path);

    if (ypath.isPort()) {
        filler(buf, "rw", NULL, 0);
        //filler(buf, "write", NULL, 0); //deprecated
        //filler(buf, "status", NULL, 0);
        return 0;
    }

    NameConfig nc;

    std::string name = nc.getNamespace();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");
    Network::write(name.c_str(),
                   msg,
                   reply);

    printf("Got %s\n", reply.toString().c_str());

    std::set<std::string> lines;


    for (int i=1; i<reply.size(); i++) {
        Bottle *entry = reply.get(i).asList();
        string rpath = path;
        if (rpath[rpath.length()-1]!='/') {
            rpath = rpath + "/";
        }
        if (entry!=NULL) {
            std::string name = entry->check("name",Value("")).asString();
            if (name!="") {
                if (strstr(name.c_str(),rpath.c_str())==
                           name.c_str()) {
                    printf(">>> %s is in path %s\n", name.c_str(),
                           rpath.c_str());
                    std::string part(name.c_str()+rpath.length());
                    if (part[0]=='/') {
                        part = part.substr(1,part.length()-1);
                    }
                    printf("    %s is the addition\n", part.c_str());

                    char *brk = (char*)strstr(part.c_str(),"/");
                    if (brk!=NULL) {
                        *brk = '\0';
                    }
                    std::string item(part.c_str());
                    printf("    %s is the item\n", item.c_str());
                    if (item!="") {
                        lines.insert(item);
                    }
                }
            }
        }
    }

    // return result in alphabetical order
    for (const auto& line : lines) {
        printf("adding item %s\n", line.c_str());
        filler(buf, line.c_str(), NULL, 0);
    }

    return 0;
}


int yarp_open(const char *path, struct fuse_file_info *fi)
{
    fi->direct_io = 1;
    fi->fh = (uint64_t) (new YHandle(path));

    if (fi->fh == 0) {
        return -EACCES;
    }

    return 0;
}


int yarp_release(const char *path, struct fuse_file_info *fi)
{
    if (YHANDLE(fi) != NULL) {
        delete YHANDLE(fi);
        fi->fh = 0;
    }
    return 0;
}


int yarp_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    size_t len;
    (void /* unused */) fi;


    YHandle *fh = YHANDLE(fi);
    if (fh==NULL) {
        return -ENOENT;
    }

    string tail = fh->getTail();
    string head = fh->getHead();
    // lets just try to implement read for now
    if (tail!="rw") {
        return -ENOENT;
    }

    return fh->read(buf,size,offset);
}


int yarp_write(const char *path, const char *buf, size_t size,
               off_t offset, struct fuse_file_info *fi)
{
    size_t len;
    (void /* unused */) fi;

    YHandle *fh = YHANDLE(fi);
    if (fh==NULL) {
        return -ENOENT;
    }

    string tail = fh->getTail();
    string head = fh->getHead();
    // lets just try to implement write for now
    if (tail!="rw") {
        return -ENOENT;
    }

    return fh->write(buf,size,offset);
}


int yarp_rename(const char *from, const char *to) {
    //TODO: the current code just renames ports, eg:
    //  /read can become /rd, but /read/rd1 cannot become /rd/rd1
    //  every subport/subdirectory should have to be renamed

    YPath ypath(from);
    if (!ypath.isPort()) { //Check that the path exists? Is it right?
        return -ENOENT;
    }


    //Create the new Contact
    Contact src = Network::queryName(from);
    Network::unregisterContact(src);

    Contact dest(to, src.getCarrier(), src.getHost(), src.getPort());

    Network::registerContact(dest);

    return 0;
}


int yarp_rmdir(const char *path) {
    //TODO: delete directories. Current code just links ports
    // eg: delete every subport/subdirectory

    YPath ypath(path);
    if (!ypath.isPort()) {
        return -ENOENT;
    }


    //Create the new Contact
    Contact src = Network::queryName(path);
    //if(!src.isValid()) {
    //    return -ENOENT;
    //}

    Network::unregisterContact(src);

    return 0;
}


int yarp_truncate(const char *path, off_t size)
{
    return 0;
}


void *yarp_init(struct fuse_conn_info *conn) {
    Network yarp;
    printf("Initializing...\n");
    return NULL;
}


static struct fuse_operations yarp_oper;

int main(int argc, char *argv[])
{
    yarp_oper.getattr  = yarp_getattr;
    yarp_oper.readdir  = yarp_readdir;
    yarp_oper.open     = yarp_open;
    yarp_oper.release  = yarp_release;
    yarp_oper.read     = yarp_read;
    yarp_oper.write    = yarp_write;
    yarp_oper.truncate = yarp_truncate;
    yarp_oper.init     = yarp_init;

    //Linking & Renaming
    yarp_oper.readlink  = yarp_readlink;
    yarp_oper.symlink   = yarp_symlink;
    yarp_oper.link      = yarp_link;
    yarp_oper.rename    = yarp_rename;

    yarp_oper.unlink    = yarp_unlink;
    yarp_oper.rmdir     =yarp_rmdir;

    return fuse_main(argc, argv, &yarp_oper, NULL);
}
