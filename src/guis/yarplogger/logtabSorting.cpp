/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
