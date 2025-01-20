/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARPRUN_PORTS_SORTING_H
#define YARPRUN_PORTS_SORTING_H

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <yarp/conf/compiler.h>

class YarprunPortsSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  YarprunPortsSortFilterProxyModel ( QObject *parent = 0 );

protected:
  bool lessThan( const QModelIndex &left, const QModelIndex &right ) const override;
};

#endif // YARPRUN_PORTS_SORTING_H
