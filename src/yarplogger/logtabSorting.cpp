/*
 * Copyright (C)2014  iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
