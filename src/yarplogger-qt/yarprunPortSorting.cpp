#include <QString>
#include <string>
#include <cstdio>
#include "yarprunPortSorting.h"

YarprunPortsSortFilterProxyModel::YarprunPortsSortFilterProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

bool YarprunPortsSortFilterProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
  int l=0;
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
      int lval = (l1 * 16777216) + (l2 * 65536) + (l3 * 256) + (l4);
      int rval = (r1 * 16777216) + (r2 * 65536) + (r3 * 256) + (r4);
      return lval < rval;
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
