// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"
#include "SubscriberOnSql.h"
#include "ParseName.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#define access(f,a) _access(f,a)
#endif

#ifndef F_OK
#define F_OK 0
#endif

#define SQLDB(x) ((sqlite3*)(x))

using namespace yarp::os;

bool SubscriberOnSql::open(const char *filename, bool fresh) {
    sqlite3 *db = NULL;
    if (fresh) {
        int result = access(filename,F_OK);
        if (result==0) {
            fprintf(stderr,"Database needs to be recreated.\n");
            fprintf(stderr,"Please move %s out of the way.\n", filename);
            return false;
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
        return false;
    }

    const char *create_subscribe_table = "CREATE TABLE IF NOT EXISTS subscriptions (\n\
	id INTEGER PRIMARY KEY,\n\
	src TEXT,\n\
	dest TEXT,\n\
	srcFull TEXT,\n\
	destFull TEXT);";

    result = sqlite3_exec(db, create_subscribe_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up subscriptions table\n");
        exit(1);
    }

    const char *create_topic_table = "CREATE TABLE IF NOT EXISTS topics (\n\
	id INTEGER PRIMARY KEY,\n\
	topic TEXT);";

    result = sqlite3_exec(db, create_topic_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up topics table\n");
        exit(1);
    }

    implementation = db;
    return true;
}


bool SubscriberOnSql::close() {
    if (implementation!=NULL) {
        sqlite3 *db = (sqlite3 *)implementation;
        sqlite3_close(db);
        implementation = NULL;
    }
    return true;
}

bool SubscriberOnSql::addSubscription(const char *src,
                                      const char *dest) {
    removeSubscription(src,dest);
    ParseName psrc, pdest;
    psrc.apply(src);
    pdest.apply(dest);
    char *msg = NULL;
    char *query = sqlite3_mprintf("INSERT INTO subscriptions (src,dest,srcFull,destFull) VALUES(%Q,%Q,%Q,%Q)", 
                                  psrc.getPortName().c_str(),
                                  pdest.getPortName().c_str(),
                                  src,
                                  dest);
    if (verbose) {
        printf("Query: %s\n", query);
    }
    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, NULL, NULL, &msg);
    if (result!=SQLITE_OK) {
        result = false;
        if (msg!=NULL) {
            fprintf(stderr,"Error: %s\n", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
    if (ok) {
        checkSubscription(psrc.getPortName().c_str(),
                          pdest.getPortName().c_str(),
                          src,
                          dest);
    }
    return ok;
}

bool SubscriberOnSql::removeSubscription(const char *src,
                                         const char *dest) {
    ParseName psrc, pdest;
    psrc.apply(src);
    pdest.apply(dest);
    char *query = sqlite3_mprintf("DELETE FROM subscriptions WHERE src = %Q AND dest = %Q",
                                  psrc.getPortName().c_str(),
                                  pdest.getPortName().c_str());
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_exec(SQLDB(implementation), query, NULL, NULL, NULL);
    bool ok = true;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
        ok = false;
    }
    sqlite3_free(query);

    return ok;
}

bool SubscriberOnSql::welcome(const char *port) {
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    //query = sqlite3_mprintf("SELECT * FROM subscriptions WHERE src = %Q OR dest= %Q",port, port);
    query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull FROM subscriptions WHERE src = %Q OR dest= %Q UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic) AND (s1.src = %Q OR s2.dest = %Q)",port, port, port, port);
    // 
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(SQLDB(implementation));
        if (msg!=NULL) {
            fprintf(stderr,"Error: %s\n", msg);
        }
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *src = (char *)sqlite3_column_text(statement,0);
        char *dest = (char *)sqlite3_column_text(statement,1);
        char *srcFull = (char *)sqlite3_column_text(statement,2);
        char *destFull = (char *)sqlite3_column_text(statement,3);
        checkSubscription(src,dest,srcFull,destFull);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);

    return false;
}


bool SubscriberOnSql::checkSubscription(const char *src,const char *dest,
                                        const char *srcFull,
                                        const char *destFull) {
    NameStore *store = getStore();
    if (store!=NULL) {
        Contact csrc = store->query(src);
        Contact cdest = store->query(dest);
        if (csrc.isValid()&&cdest.isValid()) {
            bool srcTopic = (csrc.getCarrier()=="topic");
            bool destTopic = (cdest.getCarrier()=="topic");
            if (!(srcTopic||destTopic)) {
                printf("==> trigger subscription %s %s\n", 
                       srcFull, destFull);
                connect(csrc,destFull);
                return true;
            }
        }
    }
    return false;
}

bool SubscriberOnSql::listSubscriptions(const char *port,
                                        yarp::os::Bottle& reply) {
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    if (ConstString(port)!="") {
        query = sqlite3_mprintf("SELECT * FROM subscriptions WHERE src = %Q OR dest= %Q",port,port);
    } else {
        query = sqlite3_mprintf("SELECT * FROM subscriptions");
    }
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }
    reply.addString("subscriptions");
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *src = (char *)sqlite3_column_text(statement,1);
        char *dest = (char *)sqlite3_column_text(statement,2);
        char *carrier = (char *)sqlite3_column_text(statement,5);
        Bottle& b = reply.addList();
        b.addString("subscription");
        Bottle bsrc;
        bsrc.addString("src");
        bsrc.addString(src);
        Bottle bdest;
        bdest.addString("dest");
        bdest.addString(dest);
        Bottle bcarrier;
        bcarrier.addString("carrier");
        bcarrier.addString(carrier);
        b.addList() = bsrc;
        b.addList() = bdest;
        b.addList() = bcarrier;
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);

    return true;
}


bool SubscriberOnSql::setTopic(const char *port, bool active) {
    char *query = sqlite3_mprintf("DELETE FROM topics WHERE topic = %Q",
                                  port);
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_exec(SQLDB(implementation), query, NULL, NULL, NULL);
    bool ok = true;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
        ok = false;
    }
    sqlite3_free(query);

    if (active) {
        char *msg = NULL;
        char *query = sqlite3_mprintf("INSERT INTO topics (topic) VALUES(%Q)", 
                                      port);
        if (verbose) {
            printf("Query: %s\n", query);
        }
        bool ok = true;
        int result = sqlite3_exec(SQLDB(implementation), query, 
                                  NULL, NULL, &msg);
        if (result!=SQLITE_OK) {
            result = false;
            if (msg!=NULL) {
                fprintf(stderr,"Error: %s\n", msg);
                sqlite3_free(msg);
            }
        }
        sqlite3_free(query);

        if (ok) {
            // go ahead and connect anything needed
            sqlite3_stmt *statement = NULL;
            char *query = NULL;
            query = sqlite3_mprintf("SELECT s1.src, s2.dest, s1.srcFull, s2.destFull FROM subscriptions s1, subscriptions s2, topics t WHERE (t.topic = %Q AND s1.dest = t.topic AND s2.src = t.topic)", port);
            if (verbose) {
                printf("Query: %s\n", query);
            }
            int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,
                                            &statement,
                                            NULL);
            if (result!=SQLITE_OK) {
                const char *msg = sqlite3_errmsg(SQLDB(implementation));
                if (msg!=NULL) {
                    fprintf(stderr,"Error: %s\n", msg);
                }
            }
            while (result == SQLITE_OK && 
                   sqlite3_step(statement) == SQLITE_ROW) {
                char *src = (char *)sqlite3_column_text(statement,0);
                char *dest = (char *)sqlite3_column_text(statement,1);
                char *srcFull = (char *)sqlite3_column_text(statement,2);
                char *destFull = (char *)sqlite3_column_text(statement,3);
                checkSubscription(src,dest,srcFull,destFull);
            }
            sqlite3_finalize(statement);
            sqlite3_free(query);
        }

        return ok;
    }

    return ok;
}


