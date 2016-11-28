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

#ifndef YARPRUN_PORTS_SORTING_H
#define YARPRUN_PORTS_SORTING_H

#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class YarprunPortsSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  YarprunPortsSortFilterProxyModel ( QObject *parent = 0 );

protected:
  bool lessThan( const QModelIndex &left, const QModelIndex &right ) const;
};

#endif // YARPRUN_PORTS_SORTING_H
