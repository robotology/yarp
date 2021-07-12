/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/impl/TripleSourceCreator.h>

#include <yarp/conf/compiler.h>
#include <yarp/serversql/impl/SqliteTripleSource.h>

#if !defined(_WIN32)
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
    int result = sqlite3_exec(db, cmd, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        if (msg != nullptr) {
            fprintf(stderr,"Database error: %s\n", msg);
        }
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up database tables\n");
        std::exit(1);
    }
    return true;
}


TripleSource *TripleSourceCreator::open(const char *filename,
                                        bool cautious,
                                        bool fresh) {
    sqlite3 *db = nullptr;
    if (fresh) {
        int result = access(filename,F_OK);
        if (result==0) {
            fprintf(stderr,"Database needs to be recreated.\n");
            fprintf(stderr,"Please move %s out of the way.\n", filename);
            return nullptr;
        }

    }
    int result = sqlite3_open_v2(filename,
                                 &db,
                                 SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_NOMUTEX,
                                 nullptr);
    if (result!=SQLITE_OK) {
        fprintf(stderr,"Failed to open database %s\n", filename);
        if (db != nullptr) {
            sqlite3_close(db);
        }
        return nullptr;
    }


    string create_main_table = "CREATE TABLE IF NOT EXISTS tags (\n\
    id INTEGER PRIMARY KEY,\n\
    rid INTEGER,\n\
    ns TEXT,\n\
    name TEXT,\n\
    value TEXT);";

    result = sqlite3_exec(db, create_main_table.c_str(), nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        if (msg != nullptr) {
            fprintf(stderr,"Error in %s: %s\n", filename, msg);
        }
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up database tables\n");
        std::exit(1);
    }

    string cmd_synch = string("PRAGMA synchronous=") + (cautious?"FULL":"OFF") + ";";
    sql_enact(db,cmd_synch.c_str());

    sql_enact(db,"CREATE INDEX IF NOT EXISTS tagsRidNameValue on tags(rid,name,value);");

    implementation = db;
    accessor = new SqliteTripleSource(db);
    return accessor;
}


bool TripleSourceCreator::close() {
    if (accessor != nullptr) {
        delete accessor;
        accessor = nullptr;
    }
    if (implementation != nullptr) {
        auto* db = (sqlite3 *)implementation;
        sqlite3_close(db);
        implementation = nullptr;
    }
    return true;
}
