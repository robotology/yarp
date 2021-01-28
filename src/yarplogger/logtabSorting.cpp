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

#include "logtabSorting.h"

LogSortFilterProxyModel::LogSortFilterProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

bool LogSortFilterProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
  if (left.column()==0)
  {
      //sorting by yarprun timestamp
      double leftDouble     = this->sourceModel()->data( left ).toDouble();
      double rightDouble    = this->sourceModel()->data( right ).toDouble();
      return leftDouble < rightDouble;
  }
  else if (left.column()==1)
  {
      //sorting by local timestamp
      double leftDouble     = this->sourceModel()->data( left ).toDouble();
      double rightDouble    = this->sourceModel()->data( right ).toDouble();
      return leftDouble < rightDouble;
  }

  return QSortFilterProxyModel::lessThan(left,right);
}
