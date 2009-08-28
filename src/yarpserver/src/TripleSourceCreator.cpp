// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
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


TripleSource *TripleSourceCreator::open(const char *filename, bool fresh) {
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
                                 SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,
                                 NULL);
    if (result!=SQLITE_OK) {
        fprintf(stderr,"Failed to open database %s\n", filename);
        if (db!=NULL) {
            sqlite3_close(db);
        }
        return NULL;
    }


    const char *create_main_table = "CREATE TABLE IF NOT EXISTS tags (\n\
	id INTEGER PRIMARY KEY,\n\
	rid INTEGER,\n\
	ns TEXT,\n\
	name TEXT,\n\
	value TEXT);";

    result = sqlite3_exec(db, create_main_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up database tables\n");
        exit(1);
    }

    /*
    const char *create_subscribe_table = "CREATE TABLE IF NOT EXISTS subscribe (\n\
	id INTEGER PRIMARY KEY,\n\
	source TEXT,\n\
	dest TEXT,\n\
	carrier TEXT);";

    result = sqlite3_exec(db, create_subscribe_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up database tables\n");
        exit(1);
    }
    */

    // TODO: add indices, e.g.:
    //CREATE INDEX tagsNs ON tags (ns);
    //CREATE INDEX tagsNsName ON tags (ns,name);
    //CREATE INDEX tagsName ON tags (name);

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

