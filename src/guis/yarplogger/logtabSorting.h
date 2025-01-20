/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LOGTAB_SORTING_H
#define LOGTAB_SORTING_H

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <yarp/conf/compiler.h>

class LogSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  LogSortFilterProxyModel( QObject *parent = 0 );

protected:
  bool lessThan( const QModelIndex &left, const QModelIndex &right ) const override;
};

#endif // LOGTAB_SORTING_H
