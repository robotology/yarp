#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QTreeWidget>
#include <QMenu>
class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTreeWidget(QWidget *parent = 0);
    void setContextMenu(QMenu *);

protected:
    void mousePressEvent(QMouseEvent *e);

private:
    Qt::SortOrder sort;
    QMenu *contextMenu;
signals:

private slots:
    void onHeaderClicked(int index);
    void onConnContext(QPoint p);

};

#endif // CUSTOMTREEWIDGET_H
