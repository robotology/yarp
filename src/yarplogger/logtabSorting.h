/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
