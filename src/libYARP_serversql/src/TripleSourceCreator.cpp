/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/serversql/impl/TripleSourceCreator.h>

#include <yarp/conf/compiler.h>
#include <yarp/serversql/impl/SqliteTripleSource.h>

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
using namespace yarp::serversql::impl;
using namespace std;


static bool sql_enact(sqlite3 *db, const char *cmd) {
    //printf("ISSUE %s\n", cmd);
    int result = sqlite3_exec(db, cmd, YARP_NULLPTR, YARP_NULLPTR, YARP_NULLPTR);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        if (msg != YARP_NULLPTR) {
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
    sqlite3 *db = YARP_NULLPTR;
    if (fresh) {
        int result = access(filename,F_OK);
        if (result==0) {
            fprintf(stderr,"Database needs to be recreated.\n");
            fprintf(stderr,"Please move %s out of the way.\n", filename);
            return YARP_NULLPTR;
        }

    }
    int result = sqlite3_open_v2(filename,
                                 &db,
                                 SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_NOMUTEX,
                                 YARP_NULLPTR);
    if (result!=SQLITE_OK) {
        fprintf(stderr,"Failed to open database %s\n", filename);
        if (db != YARP_NULLPTR) {
            sqlite3_close(db);
        }
        return YARP_NULLPTR;
    }


    string create_main_table = "CREATE TABLE IF NOT EXISTS tags (\n\
    id INTEGER PRIMARY KEY,\n\
    rid INTEGER,\n\
    ns TEXT,\n\
    name TEXT,\n\
    value TEXT);";

    result = sqlite3_exec(db, create_main_table.c_str(), YARP_NULLPTR, YARP_NULLPTR, YARP_NULLPTR);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        if (msg != YARP_NULLPTR) {
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
    if (accessor != YARP_NULLPTR) {
        delete accessor;
        accessor = YARP_NULLPTR;
    }
    if (implementation != YARP_NULLPTR) {
        sqlite3 *db = (sqlite3 *)implementation;
        sqlite3_close(db);
        implementation = YARP_NULLPTR;
    }
    return true;
}

