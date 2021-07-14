/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdlib>
#include <cstdio>

#include <sqlite3.h>

#include <yarp/os/RosNameSpace.h>
#include <yarp/serversql/impl/LogComponent.h>
#include <yarp/serversql/impl/SubscriberOnSql.h>
#include <yarp/serversql/impl/ParseName.h>

#if !defined(_WIN32)
#include <unistd.h>
#else
#include <io.h>
#define access(f,a) _access(f,a)
#endif

#include <vector>
#include <string>

#ifndef F_OK
#define F_OK 0
#endif

#define SQLDB(x) ((sqlite3*)(x))

using namespace yarp::os;
using namespace yarp::serversql::impl;
using namespace std;

namespace {
YARP_SERVERSQL_LOG_COMPONENT(SUBSCRIBERONSQL, "yarp.serversql.impl.SubscriberOnSql")
} // namespace


bool SubscriberOnSql::open(const std::string& filename, bool fresh) {
    sqlite3 *db = nullptr;
    if (fresh) {
        int result = access(filename.c_str(),F_OK);
        if (result==0) {
            yCWarning(SUBSCRIBERONSQL, "Database needs to be recreated.");
            yCWarning(SUBSCRIBERONSQL, "Please move %s out of the way.", filename.c_str());
            return false;
        }

    }
    int result = sqlite3_open_v2(filename.c_str(),
                                 &db,
                                 SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_NOMUTEX,
                                 nullptr);
    if (result!=SQLITE_OK) {
        yCError(SUBSCRIBERONSQL, "Failed to open database %s", filename.c_str());
        if (db != nullptr) {
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

    result = sqlite3_exec(db, create_subscribe_table, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        yCError(SUBSCRIBERONSQL, "Failed to set up subscriptions table");
        std::exit(1);
    }

    const char *check_subscriptions_size = "PRAGMA table_info(subscriptions)";

    sqlite3_stmt *statement = nullptr;
    result = sqlite3_prepare_v2(db, check_subscriptions_size, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        yCError(SUBSCRIBERONSQL, "Failed to set up subscriptions table");
        std::exit(1);
    }

    int count = 0;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        count++;
    }
    sqlite3_finalize(statement);

    if (count==5) {
        const char *add_structure = "ALTER TABLE subscriptions ADD COLUMN mode";
        result = sqlite3_exec(db, add_structure, nullptr, nullptr, nullptr);
        if (result!=SQLITE_OK) {
            sqlite3_close(db);
            yCError(SUBSCRIBERONSQL, "Failed to set up subscriptions table");
            std::exit(1);
        }
    }

    const char *create_topic_table = "CREATE TABLE IF NOT EXISTS topics (\n\
    id INTEGER PRIMARY KEY,\n\
    topic TEXT,\n\
    structure TEXT);";

    result = sqlite3_exec(db, create_topic_table, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        yCError(SUBSCRIBERONSQL, "Failed to set up topics table");
        std::exit(1);
    }

    const char *check_topic_size = "PRAGMA table_info(topics)";

    statement = nullptr;
    result = sqlite3_prepare_v2(db, check_topic_size, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        yCError(SUBSCRIBERONSQL, "Failed to set up topics table");
        std::exit(1);
    }

    count = 0;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        //sqlite3_column_text(statement,1);
        count++;
    }
    sqlite3_finalize(statement);

    if (count==2) {
        const char *add_structure = "ALTER TABLE topics ADD COLUMN structure";
        result = sqlite3_exec(db, add_structure, nullptr, nullptr, nullptr);
        if (result!=SQLITE_OK) {
            sqlite3_close(db);
            yCError(SUBSCRIBERONSQL, "Failed to set up topics table");
            std::exit(1);
        }
    }

    const char *create_live_table = "CREATE TABLE IF NOT EXISTS live (\n\
    id INTEGER PRIMARY KEY,\n\
    name TEXT UNIQUE,\n\
    stamp DATETIME);";

    result = sqlite3_exec(db, create_live_table, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        yCError(SUBSCRIBERONSQL, "Failed to set up live table");
        std::exit(1);
    }

    const char *create_struct_table = "CREATE TABLE IF NOT EXISTS structures (\n\
    name TEXT PRIMARY KEY,\n\
    yarp TEXT);";

    result = sqlite3_exec(db, create_struct_table, nullptr, nullptr, nullptr);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        yCError(SUBSCRIBERONSQL, "Failed to set up structures table");
        std::exit(1);
    }

    implementation = db;
    return true;
}


bool SubscriberOnSql::close() {
    if (implementation != nullptr) {
        auto* db = (sqlite3 *)implementation;
        sqlite3_close(db);
        implementation = nullptr;
    }
    return true;
}

bool SubscriberOnSql::addSubscription(const std::string& src,
                                      const std::string& dest,
                                      const std::string& mode) {
    removeSubscription(src,dest);
    ParseName psrc, pdest;
    psrc.apply(src);
    pdest.apply(dest);
    if (psrc.getCarrier()=="topic") {
        setTopic(psrc.getPortName(),"",true);
    }
    if (pdest.getCarrier()=="topic") {
        setTopic(pdest.getPortName(),"",true);
    }
    char *msg = nullptr;
    const char *zmode = mode.c_str();
    if (mode == "") {
        zmode = nullptr;
    }
    char *query = sqlite3_mprintf("INSERT INTO subscriptions (src,dest,srcFull,destFull,mode) VALUES(%Q,%Q,%Q,%Q,%Q)",
                                  psrc.getPortName().c_str(),
                                  pdest.getPortName().c_str(),
                                  src.c_str(),
                                  dest.c_str(),
                                  zmode);
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, nullptr, nullptr, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
    if (ok) {
        if (psrc.getCarrier()!="topic") {
            if (pdest.getCarrier()!="topic") {
                checkSubscription(psrc.getPortName(),
                                  pdest.getPortName(),
                                  src,
                                  dest,
                                  mode);
            } else {
                hookup(psrc.getPortName());
            }
        } else {
            if (pdest.getCarrier()!="topic") {
                hookup(pdest.getPortName());
            }
        }
    }
    return ok;
}

bool SubscriberOnSql::removeSubscription(const std::string& src,
                                         const std::string& dest) {
    ParseName psrc, pdest;
    psrc.apply(src);
    pdest.apply(dest);
    char *query = sqlite3_mprintf("DELETE FROM subscriptions WHERE src = %Q AND dest = %Q",
                                  psrc.getPortName().c_str(),
                                  pdest.getPortName().c_str());
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_exec(SQLDB(implementation), query, nullptr, nullptr, nullptr);
    bool ok = true;
    if (result!=SQLITE_OK) {
        yCError(SUBSCRIBERONSQL, "Error in query");
        ok = false;
    }
    sqlite3_free(query);

    return ok;
}


bool SubscriberOnSql::welcome(const std::string& port, int activity) {
    mutex.lock();

    NameSpace *ns = getDelegate();
    if (ns) {
        NestedContact nc(port);
        if (nc.getNestedName().size()>0) {
            NameStore *store = getStore();
            if (store != nullptr) {
                Contact node = store->query(nc.getNodeName());
                Contact me = store->query(port);
                if (node.isValid() && me.isValid()) {
                    if (activity>0) {
                        ns->registerAdvanced(me,store);
                    } else {
                        ns->unregisterAdvanced(port,store);
                    }
                }
            }
        }
    }

    char *msg = nullptr;
    char *query;
    if (activity>0) {
        query = sqlite3_mprintf("INSERT OR IGNORE INTO live (name,stamp) VALUES(%Q,DATETIME('now'))",
                                port.c_str());
    } else {
        // Port not responding.  Mark as non-live.
        if  (activity==0) {
            query = sqlite3_mprintf("DELETE FROM live WHERE name=%Q AND stamp < DATETIME('now','-30 seconds')",
                                    port.c_str());
        } else {
            // activity = -1 -- definite dodo
            query = sqlite3_mprintf("DELETE FROM live WHERE name=%Q",
                                    port.c_str());
        }
    }
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, nullptr, nullptr, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
    mutex.unlock();

    if (activity>0) {
        hookup(port);
    } else if (activity<0) {
        breakdown(port);
    }
    return ok;
}

bool SubscriberOnSql::hookup(const std::string& port) {
    if (getDelegate()) {
        NestedContact nc(port);
        if (nc.getNestedName().size()>0) {
            return false;
        }
    }
    mutex.lock();
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    //query = sqlite3_mprintf("SELECT * FROM subscriptions WHERE src = %Q OR dest= %Q",port, port);
    query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull FROM subscriptions WHERE (src = %Q OR dest= %Q) AND EXISTS (SELECT NULL FROM live WHERE name=src) AND EXISTS (SELECT NULL FROM live WHERE name=dest) UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic) AND (s1.src = %Q OR s2.dest = %Q) AND EXISTS (SELECT NULL FROM live WHERE name=s1.src) AND EXISTS (SELECT NULL FROM live WHERE name=s2.dest)",port.c_str(), port.c_str(), port.c_str(), port.c_str());
    //
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(SQLDB(implementation));
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
        }
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *src = (char *)sqlite3_column_text(statement,0);
        char *dest = (char *)sqlite3_column_text(statement,1);
        char *srcFull = (char *)sqlite3_column_text(statement,2);
        char *destFull = (char *)sqlite3_column_text(statement,3);
        char *mode = (char *)sqlite3_column_text(statement,4);
        checkSubscription(src,dest,srcFull,destFull,mode?mode:"");
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.unlock();

    return false;
}


bool SubscriberOnSql::breakdown(const std::string& port) {
    if (getDelegate()) {
        NestedContact nc(port);
        if (nc.getNestedName().size()>0) {
            return false;
        }
    }
    mutex.lock();
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    // query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull,mode FROM subscriptions WHERE ((src = %Q AND EXISTS (SELECT NULL FROM live WHERE name=dest)) OR (dest = %Q AND EXISTS (SELECT NULL FROM live WHERE name=src))) UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull, NULL FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic AND ((s1.src = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s2.dest)) OR (s2.dest = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s1.src))))",port, port, port, port);
    query = sqlite3_mprintf("SELECT src,dest,srcFull,destFull,mode FROM subscriptions WHERE ((src = %Q AND (mode IS NOT NULL OR EXISTS (SELECT NULL FROM live WHERE name=dest))) OR (dest = %Q AND (mode IS NOT NULL OR EXISTS (SELECT NULL FROM live WHERE name=src)))) UNION SELECT s1.src, s2.dest, s1.srcFull, s2.destFull, NULL FROM subscriptions s1, subscriptions s2, topics t WHERE (s1.dest = t.topic AND s2.src = t.topic AND ((s1.src = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s2.dest)) OR (s2.dest = %Q AND EXISTS (SELECT NULL FROM live WHERE name=s1.src))))",port.c_str(), port.c_str(), port.c_str(), port.c_str());
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(SQLDB(implementation));
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
        }
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *src = (char *)sqlite3_column_text(statement,0);
        char *dest = (char *)sqlite3_column_text(statement,1);
        char *srcFull = (char *)sqlite3_column_text(statement,2);
        char *destFull = (char *)sqlite3_column_text(statement,3);
        char *mode = (char *)sqlite3_column_text(statement,4);
        breakSubscription(port,src,dest,srcFull,destFull,mode?mode:"");
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.unlock();

    return false;
}


bool SubscriberOnSql::checkSubscription(const std::string& src,const std::string& dest,
                                        const std::string& srcFull,
                                        const std::string& destFull,
                                        const std::string& mode) {
    if (getDelegate()) {
        NestedContact nc(src);
        if (nc.getNestedName().size()>0) {
            NestedContact nc(dest);
            if (nc.getNestedName().size()>0) {
                return false;
            }
        }
    }
    yCDebug(SUBSCRIBERONSQL,
            "+++ Checking %s %s / %s %s",
            src.c_str(),
            dest.c_str(),
            srcFull.c_str(),
            destFull.c_str());

    NameStore *store = getStore();
    if (store != nullptr) {
        Contact csrc = store->query(src);
        Contact cdest = store->query(dest);
        if (csrc.isValid()&&cdest.isValid()) {
            bool srcTopic = (csrc.getCarrier()=="topic");
            bool destTopic = (cdest.getCarrier()=="topic");
            if (!(srcTopic||destTopic)) {
                yCDebug(SUBSCRIBERONSQL,
                        "++> check connection %s %s",
                        srcFull.c_str(),
                        destFull.c_str());
                connect(srcFull,destFull);
            }
        }
        if (mode!="") {
            std::string mode_name = mode;
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


bool SubscriberOnSql::breakSubscription(const std::string& dropper,
                                        const std::string& src, const std::string& dest,
                                        const std::string& srcFull,
                                        const std::string& destFull,
                                        const std::string& mode) {
    if (getDelegate()) {
        NestedContact nc(src);
        if (nc.getNestedName().size()>0) {
            NestedContact nc(dest);
            if (nc.getNestedName().size()>0) {
                return false;
            }
        }
    }
    yCDebug(SUBSCRIBERONSQL,
            "--- Checking %s %s / %s %s",
            src.c_str(),
            dest.c_str(),
            srcFull.c_str(),
            destFull.c_str());
    NameStore *store = getStore();
    if (store != nullptr) {
        bool srcDrop = std::string(dropper) == src;
        Contact contact;
        if (srcDrop) {
            contact = store->query(src);
        } else {
            contact = store->query(dest);
        }
        if (contact.isValid()) {
            yCDebug(SUBSCRIBERONSQL,
                    "--> check connection %s %s",
                    srcFull.c_str(),
                    destFull.c_str());
            disconnect(srcFull,destFull,srcDrop);
        }
        if (mode!="") {
            std::string mode_name = mode;
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



bool SubscriberOnSql::listSubscriptions(const std::string& port,
                                        yarp::os::Bottle& reply) {
    mutex.lock();
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    if (std::string(port)!="") {
        query = sqlite3_mprintf("SELECT s.srcFull, s.DestFull, EXISTS(SELECT topic FROM topics WHERE topic = s.src), EXISTS(SELECT topic FROM topics WHERE topic = s.dest), s.mode FROM subscriptions s WHERE s.src = %Q OR s.dest= %Q ORDER BY s.src, s.dest",port.c_str(),port.c_str());
    } else {
        query = sqlite3_mprintf("SELECT s.srcFull, s.destFull, EXISTS(SELECT topic FROM topics WHERE topic = s.src), EXISTS(SELECT topic FROM topics WHERE topic = s.dest), s.mode FROM subscriptions s ORDER BY s.src, s.dest");
    }
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
   if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(SQLDB(implementation));
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
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
        if (mode != nullptr) {
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
            btopic.addInt32(srcTopic);
            btopic.addInt32(destTopic);
            b.addList() = btopic;
        }
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.unlock();

    return true;
}


bool SubscriberOnSql::setTopic(const std::string& port, const std::string& structure,
                               bool active) {
    if (structure!="" || !active) {
        mutex.lock();
        char *query = sqlite3_mprintf("DELETE FROM topics WHERE topic = %Q",
                                      port.c_str());
        yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

        int result = sqlite3_exec(SQLDB(implementation), query, nullptr, nullptr, nullptr);
        bool ok = true;
        if (result!=SQLITE_OK) {
            yCError(SUBSCRIBERONSQL, "Error in query");
            ok = false;
        }
        sqlite3_free(query);
        mutex.unlock();
        if (!ok) {
            return false;
        }
        if (!active) {
            return true;
        }
    }

    bool have_topic = false;
    if (structure=="") {
        mutex.lock();
        sqlite3_stmt *statement = nullptr;
        char *query = nullptr;
        query = sqlite3_mprintf("SELECT topic FROM topics WHERE topic = %Q",
                                port.c_str());
        yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

        int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
        if (result!=SQLITE_OK) {
            yCError(SUBSCRIBERONSQL, "Error in query");
        }
        if (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            have_topic = true;
        }
        sqlite3_finalize(statement);
        sqlite3_free(query);
        mutex.unlock();
    }

    if (structure!="" || !have_topic) {
        mutex.lock();
        char *msg = nullptr;
        const char *pstructure = structure.c_str();
        if (structure == "") {
            pstructure = nullptr;
        }
        char *query = sqlite3_mprintf("INSERT INTO topics (topic,structure) VALUES(%Q,%Q)",
                                      port.c_str(),pstructure);
        yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

        bool ok = true;
        int result = sqlite3_exec(SQLDB(implementation), query, nullptr, nullptr, &msg);
        if (result!=SQLITE_OK) {
            ok = false;
            if (msg != nullptr) {
                yCError(SUBSCRIBERONSQL, "%s", msg);
                sqlite3_free(msg);
            }
        }
        sqlite3_free(query);
        mutex.unlock();
        if (!ok) {
            return false;
        }
    }

    vector<vector<std::string> > subs;

    // go ahead and connect anything needed
    mutex.lock();
    sqlite3_stmt *statement = nullptr;
    char *query = sqlite3_mprintf("SELECT s1.src, s2.dest, s1.srcFull, s2.destFull FROM subscriptions s1, subscriptions s2, topics t WHERE (t.topic = %Q AND s1.dest = t.topic AND s2.src = t.topic)", port.c_str());
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        const char *msg = sqlite3_errmsg(SQLDB(implementation));
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
        }
    }
    while (result == SQLITE_OK &&
           sqlite3_step(statement) == SQLITE_ROW) {
        char *src = (char *)sqlite3_column_text(statement,0);
        char *dest = (char *)sqlite3_column_text(statement,1);
        char *srcFull = (char *)sqlite3_column_text(statement,2);
        char *destFull = (char *)sqlite3_column_text(statement,3);
        char *mode = (char *)sqlite3_column_text(statement,4);
        vector<std::string> sub;
        sub.emplace_back(src);
        sub.emplace_back(dest);
        sub.emplace_back(srcFull);
        sub.emplace_back(destFull);
        sub.emplace_back(mode?mode:"");
        subs.push_back(sub);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.unlock();

    for (auto& sub : subs) {
        checkSubscription(sub[0],sub[1],sub[2],sub[3],sub[4]);
    }

    return true;
}


bool SubscriberOnSql::listTopics(yarp::os::Bottle& topics) {
    mutex.lock();
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    query = sqlite3_mprintf("SELECT topic FROM topics");
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
    if (result!=SQLITE_OK) {
        yCError(SUBSCRIBERONSQL, "Error in query");
    }
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *topic = (char *)sqlite3_column_text(statement,0);
        topics.addString(topic);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.unlock();

    return true;
}


bool SubscriberOnSql::setType(const std::string& family,
                              const std::string& structure,
                              const std::string& value) {
    mutex.lock();
    char *msg = nullptr;
    char *query = sqlite3_mprintf("INSERT OR REPLACE INTO structures (name,%Q) VALUES(%Q,%Q)",
                                  family.c_str(),
                                  (structure=="") ? nullptr : structure.c_str(),
                                  value.c_str());
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, nullptr, nullptr, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg != nullptr) {
            yCError(SUBSCRIBERONSQL, "%s", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);
    mutex.unlock();
    return ok;
}

std::string SubscriberOnSql::getType(const std::string& family,
                                     const std::string& structure) {
    mutex.lock();
    sqlite3_stmt *statement = nullptr;
    char *query = nullptr;
    query = sqlite3_mprintf("SELECT %s FROM structures WHERE name = %Q",
                            family.c_str(), structure.c_str());
    yCDebug(SUBSCRIBERONSQL, "Query: %s", query);

    int result = sqlite3_prepare_v2(SQLDB(implementation), query, -1, &statement, nullptr);
    std::string sresult;
    if (result!=SQLITE_OK) {
        yCError(SUBSCRIBERONSQL, "Error in query");
    }
    if (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        sresult = (const char *)sqlite3_column_text(statement,0);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex.unlock();

    return sresult;
}
