/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/serversql/impl/SqliteTripleSource.h>

#include <yarp/serversql/impl/LogComponent.h>

#include <cstdlib>
#include <cstdio>

using yarp::serversql::impl::SqliteTripleSource;
using yarp::serversql::impl::Triple;
using yarp::serversql::impl::TripleContext;

namespace {
YARP_SERVERSQL_LOG_COMPONENT(SQLITETRIPLESOURCE, "yarp.serversql.impl.SqliteTripleSource")
} // namespace

SqliteTripleSource::SqliteTripleSource(sqlite3 *db) : db(db)
{
}

std::string SqliteTripleSource::condition(Triple& t, TripleContext *context)
{
    int rid = (context != nullptr) ? context->rid : -1;
    std::string cond = "";
    if (rid==-1) {
        cond = "rid IS NULL";
    } else {
        cond = "rid = " + expressContext(context);
    }
    if (t.hasNs) {
        if (t.ns!="*") {
            char *query = nullptr;
            query = sqlite3_mprintf(" AND ns = %Q",t.getNs());
            cond = cond + query;
            sqlite3_free(query);
        }
    } else {
        cond += " AND ns IS NULL";
    }
    if (t.hasName) {
        if (t.name!="*") {
            char *query = nullptr;
            query = sqlite3_mprintf(" AND name = %Q",t.getName());
            cond = cond + query;
            sqlite3_free(query);
        }
    } else {
        cond += " AND name IS NULL";
    }
    if (t.hasValue) {
        if (t.value!="*") {
            char *query = nullptr;
            query = sqlite3_mprintf(" AND value = %Q",t.getValue());
            cond = cond + query;
            sqlite3_free(query);
        }
    } else {
        cond += " AND value IS NULL";
    }
    return cond;
}

int SqliteTripleSource::find(Triple& t, TripleContext *context)
{
    int out = -1;
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    query = sqlite3_mprintf("SELECT id FROM tags WHERE %s",
                            condition(t,context).c_str());
    yCDebug(SQLITETRIPLESOURCE, "Query: %s", query);
    int result = sqlite3_prepare_v2(db, query, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        yCWarning(SQLITETRIPLESOURCE, "Error in query");
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        if (out!=-1) {
            yCWarning(SQLITETRIPLESOURCE, "WARNING: multiple matches ignored");
        }
        out = sqlite3_column_int(statement,0);
        yCTrace(SQLITETRIPLESOURCE, "Match %d", out);
    }

    sqlite3_finalize(statement);
    sqlite3_free(query);
    return out;
}

void SqliteTripleSource::remove_query(Triple& ti, TripleContext *context)
{
    char *query = nullptr;
    query = sqlite3_mprintf("DELETE FROM tags WHERE %s",condition(ti,context).c_str());
    yCDebug(SQLITETRIPLESOURCE, "Query: %s", query);
    int result = sqlite3_exec(db, query, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        yCWarning(SQLITETRIPLESOURCE, "Error in query");
    }
    sqlite3_free(query);
}

void SqliteTripleSource::prune(TripleContext *context)
{
    char *query = nullptr;
    query = sqlite3_mprintf("DELETE FROM tags WHERE rid IS NOT NULL AND rid  NOT IN (SELECT id FROM tags)");
    yCDebug(SQLITETRIPLESOURCE, "Query: %s", query);
    int result = sqlite3_exec(db, query, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        yCWarning(SQLITETRIPLESOURCE, "Error in query");
    }
    sqlite3_free(query);
}

std::list<Triple> SqliteTripleSource::query(Triple& ti, TripleContext *context)
{
    std::list<Triple> q;
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    query = sqlite3_mprintf("SELECT id, ns, name, value FROM tags WHERE %s",condition(ti,context).c_str());
    yCDebug(SQLITETRIPLESOURCE, "Query: %s", query);
    int result = sqlite3_prepare_v2(db, query, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        yCWarning(SQLITETRIPLESOURCE, "Error in query");
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        //int id = sqlite3_column_int(statement,0);
        char *ns = (char *)sqlite3_column_text(statement,1);
        char *name = (char *)sqlite3_column_text(statement,2);
        char *value = (char *)sqlite3_column_text(statement,3);
        Triple t;
        if (ns != nullptr) {
            t.ns = ns;
            t.hasNs = true;
        }
        if (name != nullptr) {
            t.name = name;
            t.hasName = true;
        }
        if (value != nullptr) {
            t.value = value;
            t.hasValue = true;
        }
        q.push_back(t);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    return q;
}

std::string SqliteTripleSource::expressContext(TripleContext *context)
{
    int rid = (context != nullptr)?context->rid:-1;
    char buf[100] = "NULL";
    if (rid!=-1) {
        std::snprintf(buf, 100, "%d", rid);
    }
    return buf;
}

void SqliteTripleSource::insert(Triple& t, TripleContext *context)
{
    char *msg = nullptr;
    char *query = sqlite3_mprintf("INSERT INTO tags (rid,ns,name,value) VALUES(%s,%Q,%Q,%Q)",
                                  expressContext(context).c_str(),
                                  t.getNs(),t.getName(),t.getValue());
    yCDebug(SQLITETRIPLESOURCE, "Query: %s", query);
    int result = sqlite3_exec(db, query, nullptr, nullptr, &msg);
    if (result!=SQLITE_OK) {
        if (msg != nullptr) {
            yCError(SQLITETRIPLESOURCE, "Error: %s", msg);
            yCError(SQLITETRIPLESOURCE, "(Query was): %s", query);
            yCError(SQLITETRIPLESOURCE, "(Location): %s:%d", __FILE__, __LINE__);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
}

void SqliteTripleSource::update(Triple& t, TripleContext *context)
{
    char *msg = nullptr;
    char *query = nullptr;
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
    yCDebug(SQLITETRIPLESOURCE, "Query: %s", query);
    int result = sqlite3_exec(db, query, nullptr, nullptr, &msg);
    if (result!=SQLITE_OK) {
        if (msg != nullptr) {
            yCError(SQLITETRIPLESOURCE, "Error: %s", msg);
            sqlite3_free(msg);
        }
    }
    int ct = sqlite3_changes(db);
    if (ct==0 && (t.hasName||t.hasNs)) {
        insert(t,context);
    }
    sqlite3_free(query);
}

void SqliteTripleSource::begin(TripleContext *context)
{
    int result = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    yCDebug(SQLITETRIPLESOURCE, "Query: BEGIN TRANSACTION;");
    if (result!=SQLITE_OK) {
        yCWarning(SQLITETRIPLESOURCE, "Error in BEGIN query");
    }
}

void SqliteTripleSource::end(TripleContext *context)
{
    int result = sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr);
    yCDebug(SQLITETRIPLESOURCE, "Query: END TRANSACTION;");
    if (result!=SQLITE_OK) {
        yCWarning(SQLITETRIPLESOURCE, "Error in END query");
    }
}
