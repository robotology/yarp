// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "TripleSourceCreator.h"
#include "SqliteTripleSource.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#define access(f,a) _access(f,a)
#endif

#ifndef F_OK
#define F_OK 0
#endif

#include <string>
using namespace std;


static bool sql_enact(sqlite3 *db, const char *cmd) {
    //printf("ISSUE %s\n", cmd);
    int result = sqlite3_exec(db, cmd, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        if (msg!=NULL) {
            fprintf(stderr,"Database error: %s\n", msg);
        }
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up database tables\n");
        exit(1);
    }
    return true;
}


TripleSource *TripleSourceCreator::open(const char *filename, 
                                        bool cautious,
                                        bool fresh) {
    sqlite3 *db = NULL;
    if (fresh) {
        int result = access(filename,F_OK);
        if (result==0) {
            fprintf(stderr,"Database needs to be recreated.\n");
            fprintf(stderr,"Please move %s out of the way.\n", filename);
            return NULL;
        }

    }
    int result = sqlite3_open_v2(filename,
                                 &db,
                                 SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|
                                 SQLITE_OPEN_NOMUTEX,
                                 NULL);
    if (result!=SQLITE_OK) {
        fprintf(stderr,"Failed to open database %s\n", filename);
        if (db!=NULL) {
            sqlite3_close(db);
        }
        return NULL;
    }


    string create_main_table = "CREATE TABLE IF NOT EXISTS tags (\n\
	id INTEGER PRIMARY KEY,\n\
	rid INTEGER,\n\
	ns TEXT,\n\
	name TEXT,\n\
	value TEXT);";

    result = sqlite3_exec(db, create_main_table.c_str(), NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        if (msg!=NULL) {
            fprintf(stderr,"Error in %s: %s\n", filename, msg);
        }
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up database tables\n");
        exit(1);
    }

    string cmd_synch = string("PRAGMA synchronous=") + (cautious?"FULL":"OFF") + ";";
    sql_enact(db,cmd_synch.c_str());

    sql_enact(db,"CREATE INDEX IF NOT EXISTS tagsRidNameValue on tags(rid,name,value);");

    implementation = db;
    accessor = new SqliteTripleSource(db);
    return accessor;
}


bool TripleSourceCreator::close() {
    if (accessor!=NULL) {
        delete accessor;
        accessor = NULL;
    }
    if (implementation!=NULL) {
        sqlite3 *db = (sqlite3 *)implementation;
        sqlite3_close(db);
        implementation = NULL;
    }
    return true;
}

