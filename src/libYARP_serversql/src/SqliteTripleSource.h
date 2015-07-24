// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_SQLITELIVETRIPLE_INC
#define YARPDB_SQLITELIVETRIPLE_INC

#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"
#include "Triple.h"
#include "TripleSource.h"

#ifdef _MSC_VER
#define safe_printf sprintf_s
#else
#define safe_printf snprintf
#endif 

/**
 *
 * Sqlite database, viewed as a collection of triples.  These are the
 * minimum functions needed by the name server to use a Sqlite
 * database.
 *
 */
class SqliteTripleSource : public TripleSource {
private:
    sqlite3 *db;
public:
    SqliteTripleSource(sqlite3 *db) : db(db) {
    }

    std::string condition(Triple& t, TripleContext *context) {
        int rid = (context!=NULL)?context->rid:-1;
        std::string cond = "";
        if (rid==-1) {
            cond = "rid IS NULL";
        } else {
            cond = "rid = " + expressContext(context);
        }
        if (t.hasNs) {
            if (t.ns!="*") {
                char *query = NULL;
                query = sqlite3_mprintf(" AND ns = %Q",t.getNs());
                cond = cond + query;
                sqlite3_free(query);
            }
        } else {
            cond += " AND ns IS NULL";
        }
        if (t.hasName) {
            if (t.name!="*") {
                char *query = NULL;
                query = sqlite3_mprintf(" AND name = %Q",t.getName());
                cond = cond + query;
                sqlite3_free(query);
            }
        } else {
            cond += " AND name IS NULL";
        }
        if (t.hasValue) {
            if (t.value!="*") {
                char *query = NULL;
                query = sqlite3_mprintf(" AND value = %Q",t.getValue());
                cond = cond + query;
                sqlite3_free(query);
            }
        } else {
            cond += " AND value IS NULL";
        }
        return cond;
    }

    int find(Triple& t, TripleContext *context) {
        int out = -1;
        sqlite3_stmt *statement = NULL;
        char *query = NULL;
        query = sqlite3_mprintf("SELECT id FROM tags WHERE %s",
                                condition(t,context).c_str());
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_prepare_v2(db,query,-1,&statement,NULL);
        if (result!=SQLITE_OK) {
            printf("Error in query\n");
        }
        while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            if (out!=-1) {
                fprintf(stderr,"*** WARNING: multiple matches ignored\n");
            }
            out = sqlite3_column_int(statement,0);
            //printf("Match %d\n", out);
        }
        //if (out==-1) {
        //printf("No match for %s\n", t.toString().c_str());
        //}
        sqlite3_finalize(statement);
        sqlite3_free(query);
        return out;
    }

    void remove_query(Triple& ti, TripleContext *context) {
        char *query = NULL;
        query = sqlite3_mprintf("DELETE FROM tags WHERE %s",condition(ti,context).c_str());
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_exec(db, query, NULL, NULL, NULL);
        if (result!=SQLITE_OK) {
            printf("Error in query\n");
        }
        sqlite3_free(query);
    }

    void prune(TripleContext *context) {
        char *query = NULL;
        query = sqlite3_mprintf("DELETE FROM tags WHERE rid IS NOT NULL AND rid  NOT IN (SELECT id FROM tags)");
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_exec(db, query, NULL, NULL, NULL);
        if (result!=SQLITE_OK) {
            printf("Error in query\n");
        }
        sqlite3_free(query);
    }

    std::list<Triple> query(Triple& ti, TripleContext *context) {
        std::list<Triple> q;
        sqlite3_stmt *statement = NULL;
        char *query = NULL;
        query = sqlite3_mprintf("SELECT id, ns, name, value FROM tags WHERE %s",condition(ti,context).c_str());
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_prepare_v2(db,query,-1,&statement,NULL);
        if (result!=SQLITE_OK) {
            printf("Error in query\n");
        }
        while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            //int id = sqlite3_column_int(statement,0);
            char *ns = (char *)sqlite3_column_text(statement,1);
            char *name = (char *)sqlite3_column_text(statement,2);
            char *value = (char *)sqlite3_column_text(statement,3);
            Triple t;
            if (ns!=NULL) {
                t.ns = ns;
                t.hasNs = true;
            }
            if (name!=NULL) {
                t.name = name;
                t.hasName = true;
            }
            if (value!=NULL) {
                t.value = value;
                t.hasValue = true;
            }
            q.push_back(t);
        }
        sqlite3_finalize(statement);
        sqlite3_free(query);
        return q;
    }

    std::string expressContext(TripleContext *context) {
        int rid = (context!=NULL)?context->rid:-1;
        char buf[100] = "NULL";
        if (rid!=-1) {
            safe_printf(buf,100,"%d",rid);
        }
        return buf;
    }

    void insert(Triple& t, TripleContext *context) {
        char *msg = NULL;
        char *query = sqlite3_mprintf("INSERT INTO tags (rid,ns,name,value) VALUES(%s,%Q,%Q,%Q)", 
                                      expressContext(context).c_str(),
                                      t.getNs(),t.getName(),t.getValue());
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_exec(db, query, NULL, NULL, &msg);
        if (result!=SQLITE_OK) {
            if (msg!=NULL) {
                fprintf(stderr,"Error: %s\n", msg);
                fprintf(stderr,"(Query was): %s\n", query);
                fprintf(stderr,"(Location): %s:%d\n", __FILE__, __LINE__);
                if (verbose) {
                    exit(1);
                }
                sqlite3_free(msg);
            }
        }
        sqlite3_free(query);
    }

    void update(Triple& t, TripleContext *context) {
        char *msg = NULL;
        char *query = NULL;
        if (t.hasName||t.hasNs) {
            Triple t2(t);
            t2.value = "*";
            query = sqlite3_mprintf("UPDATE tags SET value = %Q WHERE %s", 
                                    t.getValue(),
                                    condition(t2,context).c_str());
        } else {
            query = sqlite3_mprintf("UPDATE tags SET value = %Q WHERE id = %Q", 
                                    t.getValue(),
                                    expressContext(context).c_str());
        }
        if (verbose) {
            printf("Query: %s\n", query);
        }
        int result = sqlite3_exec(db, query, NULL, NULL, &msg);
        if (result!=SQLITE_OK) {
            if (msg!=NULL) {
                fprintf(stderr,"Error: %s\n", msg);
                sqlite3_free(msg);
            }
        }
        int ct = sqlite3_changes(db);
        if (ct==0 && (t.hasName||t.hasNs)) {
            insert(t,context);
        }
        sqlite3_free(query);
    }


    virtual void begin(TripleContext *context) {
        int result = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        if (verbose) {
            printf("Query: BEGIN TRANSACTION;\n");
        }
        if (result!=SQLITE_OK) {
            printf("Error in BEGIN query\n");
        }
    }

    virtual void end(TripleContext *context) {
        int result = sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
        if (verbose) {
            printf("Query: END TRANSACTION;\n");
        }
        if (result!=SQLITE_OK) {
            printf("Error in END query\n");
        }
    }

};

#endif
