// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
                                 SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|
                                 SQLITE_OPEN_NOMUTEX,
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
	destFull TEXT,\n\
    mode TEXT);";

    result = sqlite3_exec(db, create_subscribe_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up subscriptions table\n");
        exit(1);
    }

    const char *check_subscriptions_size = "PRAGMA table_info(subscriptions)";

    sqlite3_stmt *statement = NULL;
    result = sqlite3_prepare_v2(db, check_subscriptions_size, -1, 
                                &statement, NULL);
    if (result!=SQLITE_OK) {
        fprintf(stderr,"Failed to set up subscriptions table\n");
        exit(1);
    }
    
    int count = 0;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        count++;
    }
    sqlite3_finalize(statement);
    
    if (count==5) {
        const char *add_structure = "ALTER TABLE subscriptions ADD COLUMN mode";
        result = sqlite3_exec(db, add_structure, NULL, NULL, NULL);
        if (result!=SQLITE_OK) {
            sqlite3_close(db);
            fprintf(stderr,"Failed to set up subscriptions table\n");
            exit(1);
        }
    }

    const char *create_topic_table = "CREATE TABLE IF NOT EXISTS topics (\n\
	id INTEGER PRIMARY KEY,\n\
	topic TEXT,\n\
    structure TEXT);";

    result = sqlite3_exec(db, create_topic_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up topics table\n");
        exit(1);
    }

    const char *check_topic_size = "PRAGMA table_info(topics)";

    statement = NULL;
    result = sqlite3_prepare_v2(db, check_topic_size, -1, 
                                &statement, NULL);
    if (result!=SQLITE_OK) {
        fprintf(stderr,"Failed to set up topics table\n");
        exit(1);
    }
    
    count = 0;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        //sqlite3_column_text(statement,1);
        count++;
    }
    sqlite3_finalize(statement);
    
    if (count==2) {
        const char *add_structure = "ALTER TABLE topics ADD COLUMN structure";
        result = sqlite3_exec(db, add_structure, NULL, NULL, NULL);
        if (result!=SQLITE_OK) {
            sqlite3_close(db);
            fprintf(stderr,"Failed to set up topics table\n");
            exit(1);
        }
    }

    const char *create_live_table = "CREATE TABLE IF NOT EXISTS live (\n\
	id INTEGER PRIMARY KEY,\n\
	name TEXT UNIQUE,\n\
    stamp DATETIME);";

    result = sqlite3_exec(db, create_live_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up live table\n");
        exit(1);
    }

    const char *create_struct_table = "CREATE TABLE IF NOT EXISTS structures (\n\
	name TEXT PRIMARY KEY,\n\
    yarp TEXT);";

    result = sqlite3_exec(db, create_struct_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up structures table\n");
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
                                      const char *dest,
                                      const char *mode) {
    removeSubscription(src,dest);
    ParseName psrc, pdest;
    psrc.apply(src);
    pdest.apply(dest);
    if (psrc.getCarrier()=="topic") {
        setTopic(psrc.getPortName().c_str(),NULL,true);
    }
    if (pdest.getCarrier()=="topic") {
        setTopic(pdest.getPortName().c_str(),NULL,true);
    }
    char *msg = NULL;
    const char *zmode = mode;
    if (mode[0] == '\0') zmode = NULL;
    char *query = sqlite3_mprintf("INSERT INTO subscriptions (src,dest,srcFull,destFull,mode) VALUES(%Q,%Q,%Q,%Q,%Q)", 
                                  psrc.getPortName().c_str(),
                                  pdest.getPortName().c_str(),
                                  src,
                                  dest,
                                  zmode);
    if (verbose) {
        printf("Query: %s\n", query);
    }
    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, NULL, NULL, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg!=NULL) {
            fprintf(stderr,"Error: %s\n", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
    if (ok) {
        if (psrc.getCarrier()!="topic") {
            if (pdest.getCarrier()!="topic") {
                checkSubscription(psrc.getPortName().c_str(),
                                  pdest.getPortName().c_str(),
                                  src,
                                  dest,
                                  zmode);
            } else {
                hookup(psrc.getPortName().c_str());
            }
        } else {
            if (pdest.getCarrier()!="topic") {
                hookup(pdest.getPortName().c_str());
            }
        }
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

bool SubscriberOnSql::welcome(const char *port, int activity) {
    mutex.wait();
    char *msg = NULL;
    char *query;
    if (activity>0) {
        query = sqlite3_mprintf("INSERT OR IGNORE INTO live (name,stamp) VALUES(%Q,DATETIME('now'))", 
                                port);
    } else {
        // Port not responding.  Mark as non-live.
        if  (activity==0) {
            query = sqlite3_mprintf("DELETE FROM live WHERE name=%Q AND stamp < DATETIME('now','-30 seconds')", 
                                    port);
        } else {
            // activity = -1 -- definite dodo
            query = sqlite3_mprintf("DELETE FROM live WHERE name=%Q", 
                                    port);
        }
    }
    if (verbose) {
        printf("Query: %s\n", query);
    }
    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, 
                              NULL, NULL, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg!=NULL) {
            fprintf(stderr,"Error: %s\n", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
    mutex.post();

    if (activity>0) {
        hookup(port);
    } else if (activity<0) {
        breakdown(port);
    }
    return ok;
}

bool SubscriberOnSql::hookup(const char *port) {
    mutex.wait();
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    //query = sqlite3_mprintf("SELECT * FROM subscriptions WHERE src = %Q OR dest= %Q",port, port);
    query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull FROM subscriptions WHERE (src = %Q OR dest= %Q) AND EXISTS (SELECT NULL FROM live WHERE name=src) AND EXISTS (SELECT NULL FROM live WHERE name=dest) UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic) AND (s1.src = %Q OR s2.dest = %Q) AND EXISTS (SELECT NULL FROM live WHERE name=s1.src) AND EXISTS (SELECT NULL FROM live WHERE name=s2.dest)",port, port, port, port);
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
        char *mode = (char *)sqlite3_column_text(statement,4);
        checkSubscription(src,dest,srcFull,destFull,mode);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.post();

    return false;
}


bool SubscriberOnSql::breakdown(const char *port) {
    mutex.wait();
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    // query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull,mode FROM subscriptions WHERE ((src = %Q AND EXISTS (SELECT NULL FROM live WHERE name=dest)) OR (dest = %Q AND EXISTS (SELECT NULL FROM live WHERE name=src))) UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull, NULL FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic AND ((s1.src = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s2.dest)) OR (s2.dest = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s1.src))))",port, port, port, port);
    query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull,mode FROM subscriptions WHERE ((src = %Q AND (mode IS NOT NULL OR EXISTS (SELECT NULL FROM live WHERE name=dest))) OR (dest = %Q AND (mode IS NOT NULL OR EXISTS (SELECT NULL FROM live WHERE name=src)))) UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull, NULL FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic AND ((s1.src = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s2.dest)) OR (s2.dest = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s1.src))))",port, port, port, port);
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
        char *mode = (char *)sqlite3_column_text(statement,4);
        breakSubscription(port,src,dest,srcFull,destFull,mode);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.post();

    return false;
}


bool SubscriberOnSql::checkSubscription(const char *src,const char *dest,
                                        const char *srcFull,
                                        const char *destFull,
                                        const char *mode) {
    if (verbose) {
        printf("+++ Checking %s %s / %s %s\n",
               src, dest, srcFull, destFull);
    }
    NameStore *store = getStore();
    if (store!=NULL) {
        Contact csrc = store->query(src);
        Contact cdest = store->query(dest);
        if (csrc.isValid()&&cdest.isValid()) {
            bool srcTopic = (csrc.getCarrier()=="topic");
            bool destTopic = (cdest.getCarrier()=="topic");
            if (!(srcTopic||destTopic)) {
                printf("++> check connection %s %s\n", 
                       srcFull, destFull);
                connect(srcFull,destFull);
            }
        }
        if (mode!=NULL) {
            ConstString mode_name = mode;
            if (mode_name=="from") {
                if (!csrc.isValid()) {
                    removeSubscription(src,dest);
                }
            } else if (mode_name=="to") {
                if (!cdest.isValid()) {
                    removeSubscription(src,dest);
                }
            }
        }
    }
    return false;
}


bool SubscriberOnSql::breakSubscription(const char *dropper,
                                        const char *src, const char *dest,
                                        const char *srcFull, 
                                        const char *destFull,
                                        const char *mode) {
    if (verbose) {
        printf("--- Checking %s %s / %s %s\n",
               src, dest, srcFull, destFull);
    }
    NameStore *store = getStore();
    if (store!=NULL) {
        bool srcDrop = ConstString(dropper) == src;
        Contact contact;
        if (srcDrop) {
            contact = store->query(src);
        } else {
            contact = store->query(dest);
        }
        if (contact.isValid()) {
            printf("--> check connection %s %s\n", 
                   srcFull, destFull);
            disconnect(srcFull,destFull,srcDrop);
        }
        if (mode!=NULL) {
            ConstString mode_name = mode;
            if (mode_name=="from") {
                if (srcDrop) {
                    removeSubscription(src,dest);
                }
            } else if (mode_name=="to") {
                if (!srcDrop) {
                    removeSubscription(src,dest);
                }
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
        query = sqlite3_mprintf("SELECT s.srcFull, s.DestFull, EXISTS(SELECT topic FROM topics WHERE topic = s.src), EXISTS(SELECT topic FROM topics WHERE topic = s.dest), s.mode FROM subscriptions s WHERE s.src = %Q OR s.dest= %Q ORDER BY s.src, s.dest",port,port);
    } else {
        query = sqlite3_mprintf("SELECT s.srcFull, s.destFull, EXISTS(SELECT topic FROM topics WHERE topic = s.src), EXISTS(SELECT topic FROM topics WHERE topic = s.dest), s.mode FROM subscriptions s ORDER BY s.src, s.dest");
    }
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
    reply.addString("subscriptions");
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *src = (char *)sqlite3_column_text(statement,0);
        char *dest = (char *)sqlite3_column_text(statement,1);
        int srcTopic = sqlite3_column_int(statement,2);
        int destTopic = sqlite3_column_int(statement,3);
        char *mode = (char *)sqlite3_column_text(statement,4);
        Bottle& b = reply.addList();
        b.addString("subscription");
        Bottle bsrc;
        bsrc.addString("src");
        bsrc.addString(src);
        Bottle bdest;
        bdest.addString("dest");
        bdest.addString(dest);
        b.addList() = bsrc;
        b.addList() = bdest;
        if (mode!=NULL) {
            if (mode[0]!='\0') {
                Bottle bmode;
                bmode.addString("mode");
                bmode.addString(mode);
                b.addList() = bmode;
            }
        }
        if (srcTopic||destTopic) {
            Bottle btopic;
            btopic.addString("topic");
            btopic.addInt(srcTopic);
            btopic.addInt(destTopic);
            b.addList() = btopic;
        }
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);

    return true;
}


bool SubscriberOnSql::setTopic(const char *port, const char *structure,
                               bool active) {
    if (structure!=NULL || !active) {
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
        if (!ok) return false;
        if (!active) return true;
    }

    bool have_topic = false;
    if (structure==NULL) {
        sqlite3_stmt *statement = NULL;
        char *query = NULL;
        query = sqlite3_mprintf("SELECT topic FROM topics WHERE topic = %Q",
                                port);
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                        NULL);
        if (result!=SQLITE_OK) {
            printf("Error in query\n");
        }
        if (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            have_topic = true;
        }
        sqlite3_finalize(statement);
        sqlite3_free(query);
    }

    if (structure!=NULL || !have_topic) {
        char *msg = NULL;
        if (structure!=NULL) {
            if (structure[0]=='\0') structure = NULL;
        }
        char *query = sqlite3_mprintf("INSERT INTO topics (topic,structure) VALUES(%Q,%Q)", 
                                      port,structure);
        if (verbose) {
            printf("Query: %s\n", query);
        }
        bool ok = true;
        int result = sqlite3_exec(SQLDB(implementation), query, 
                                  NULL, NULL, &msg);
        if (result!=SQLITE_OK) {
            ok = false;
            if (msg!=NULL) {
                fprintf(stderr,"Error: %s\n", msg);
                sqlite3_free(msg);
            }
        }
        sqlite3_free(query);    
        if (!ok) return false;
    }

    // go ahead and connect anything needed
    sqlite3_stmt *statement = NULL;
    char *query = sqlite3_mprintf("SELECT s1.src, s2.dest, s1.srcFull, s2.destFull FROM subscriptions s1, subscriptions s2, topics t WHERE (t.topic = %Q AND s1.dest = t.topic AND s2.src = t.topic)", port);
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
        char *mode = (char *)sqlite3_column_text(statement,4);
        checkSubscription(src,dest,srcFull,destFull,mode);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);

    return true;
}


bool SubscriberOnSql::listTopics(yarp::os::Bottle& topics) {
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    query = sqlite3_mprintf("SELECT topic FROM topics");
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *topic = (char *)sqlite3_column_text(statement,0);
        topics.addString(topic);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);

    return true;
}


bool SubscriberOnSql::setType(const char *family,
                              const char *structure,
                              const char *value) {
    char *msg = NULL;
    if (structure!=NULL) {
        if (structure[0]=='\0') structure = NULL;
    }
    char *query = sqlite3_mprintf("INSERT OR REPLACE INTO structures (name,%Q) VALUES(%Q,%Q)", 
                                  family,structure,value);
    if (verbose) {
        printf("Query: %s\n", query);
    }
    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, 
                              NULL, NULL, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg!=NULL) {
            fprintf(stderr,"Error: %s\n", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);    
    return ok;
}

ConstString SubscriberOnSql::getType(const char *family,
                                     const char *structure) {
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    query = sqlite3_mprintf("SELECT %s FROM structures WHERE name = %Q",
                            family, structure);
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    ConstString sresult;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }
    if (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        sresult = (const char *)sqlite3_column_text(statement,0);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    
    return sresult;
}
