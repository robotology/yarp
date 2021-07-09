/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SERVERSQL_IMPL_SQLITETRIPLESOURCE_H
#define YARP_SERVERSQL_IMPL_SQLITETRIPLESOURCE_H

#include <yarp/serversql/impl/TripleSource.h>
#include <yarp/serversql/impl/Triple.h>

#include <sqlite3.h>

namespace yarp {
namespace serversql {
namespace impl {

/**
 * Sqlite database, viewed as a collection of triples.  These are the
 * minimum functions needed by the name server to use a Sqlite
 * database.
 */
class SqliteTripleSource : public TripleSource
{
public:
    SqliteTripleSource(sqlite3 *db);

    std::string condition(Triple& t, TripleContext *context);

    int find(Triple& t, TripleContext *context) override;
    void remove_query(Triple& ti, TripleContext *context) override;
    void prune(TripleContext *context) override;
    std::list<Triple> query(Triple& ti, TripleContext *context) override;
    std::string expressContext(TripleContext *context);
    void insert(Triple& t, TripleContext *context) override;
    void update(Triple& t, TripleContext *context) override;
    void begin(TripleContext *context) override;
    void end(TripleContext *context) override;

private:
    sqlite3 *db;
};

} // namespace impl
} // namespace serversql
} // namespace yarp


#endif // YARP_SERVERSQL_IMPL_SQLITETRIPLESOURCE_H
