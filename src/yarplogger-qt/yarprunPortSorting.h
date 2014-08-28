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
