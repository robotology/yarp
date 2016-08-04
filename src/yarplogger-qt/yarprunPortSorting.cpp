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

#include <QString>
#include <string>
#include <cstdio>
#include <ctime>
#include "yarprunPortSorting.h"

YarprunPortsSortFilterProxyModel::YarprunPortsSortFilterProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

bool YarprunPortsSortFilterProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
  if (left.column()==0)
  {
      //sorting by ip
      int l1,l2,l3,l4;
      int r1,r2,r3,r4;
      QString leftIp     = this->sourceModel()->data( left ).toString();
      QString rightIp    = this->sourceModel()->data( right ).toString();
      leftIp.replace('.', ' ');
      rightIp.replace('.', ' ');
      int lres = sscanf  (leftIp.toStdString().c_str(),"%d %d %d %d",&l1,&l2,&l3,&l4);
      int rres = sscanf (rightIp.toStdString().c_str(),"%d %d %d %d",&r1,&r2,&r3,&r4);
      Q_UNUSED(lres);
      Q_UNUSED(rres);
      int lval = (l1 * 16777216) + (l2 * 65536) + (l3 * 256) + (l4);
      int rval = (r1 * 16777216) + (r2 * 65536) + (r3 * 256) + (r4);
      return lval < rval;
  }
  else if (left.column()==2)
  {
      QString leftStr     = this->sourceModel()->data( left ).toString();
      QString rightStr    = this->sourceModel()->data( right ).toString();
      struct tm left_time = { 0 };
      struct tm right_time = { 0 };
      sscanf(leftStr.toStdString().c_str(), "%d:%d:%d %d/%d/%d", &left_time.tm_hour, &left_time.tm_min, &left_time.tm_sec, &left_time.tm_mday, &left_time.tm_mon, &left_time.tm_year);
      sscanf(rightStr.toStdString().c_str(), "%d:%d:%d %d/%d/%d", &right_time.tm_hour, &right_time.tm_min, &right_time.tm_sec, &right_time.tm_mday, &right_time.tm_mon, &right_time.tm_year);
      left_time.tm_year-=1900;
      right_time.tm_year-=1900;
      double diff = difftime(mktime(&left_time), mktime(&right_time));
      return diff > 0;
  }
  else if (left.column()==3 || left.column()==4 || left.column()==5)
  {
      //sorting by log size
      int leftInt     = this->sourceModel()->data( left ).toInt();
      int rightInt    = this->sourceModel()->data( right ).toInt();
      return leftInt < rightInt;
  }

  return QSortFilterProxyModel::lessThan(left,right);
}
