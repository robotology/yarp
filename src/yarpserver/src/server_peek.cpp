// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "TripleSourceCreator.h"

using namespace std;

int main(int argc, char *argv[]) {
    TripleSourceCreator db;
    TripleSource *pmem = db.open("yarp.db");
    if (pmem == NULL) {
        fprintf(stderr,"Aborting, no database found\n");
        return 1;
    }
    TripleSource& mem = *pmem;
    string mode = "";
    TripleContext context;
    for (int i=1; i<argc; i++) {
        string arg = argv[i];
        if (arg[0]=='-') {
            mode = arg;
            if (mode == "--verbose") {
                mem.setVerbose(1);
            } else if (mode=="--prune") {
                mem.prune(&context);
            }
        } else {
            if (mode=="--with") {
                Triple t;
                t.split(arg);
                int result = mem.find(t,&context);
                context.setRid(result);
                mode = "";
            } else if (mode=="--query") {
                Triple t;
                t.split(arg);
                list<Triple> q = mem.query(t,&context);
                for (list<Triple>::iterator it=q.begin(); it!=q.end(); it++) {
                    printf("*** %s\n", (*it).toString().c_str());
                }
                if (q.size()==0) {
                    printf("No match for %s\n", t.toString().c_str());
                }
            } else if (mode=="--delete") {
                Triple t;
                t.split(arg);
                mem.remove_query(t,&context);
            } else if (mode=="--add") {
                Triple t;
                t.split(arg);
                mem.insert(t,&context);
            } else if (mode=="--set") {
                Triple t;
                t.split(arg);
                mem.update(t,&context);
            } else {
                printf("Not sure what to do with [%s]\n", arg.c_str());
            }
        }
        
    }

    db.close();

    return 0;
}

