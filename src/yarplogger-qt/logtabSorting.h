#ifndef LOGTAB_SORTING_H
#define LOGTAB_SORTING_H

#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class LogSortFilterProxyModel : public QSortFilterProxyModel
{
public:
  LogSortFilterProxyModel( QObject *parent = 0 );

protected:
  bool lessThan( const QModelIndex &left, const QModelIndex &right ) const;
};

#endif // LOGTAB_SORTING_H
