#ifndef DESTINATIONPORTITEM_H
#define DESTINATIONPORTITEM_H

#include <QObject>
#include <QPainter>
#include <QGraphicsPolygonItem>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "commons.h"
#include "arrow.h"
#include "itemsignalhandler.h"
#include "builderitem.h"

class ItemSignalHandler;
class Arrow;

class DestinationPortItem : public BuilderItem
{
    friend class Arrow;
public:
    DestinationPortItem(QString itemName,bool nestedInApp = false,
                        bool editOnStart = false, Application *app = NULL,BuilderItem * parent = 0);
    ~DestinationPortItem();
    QRectF boundingRect() const;
    QPointF connectionPoint();
    int type() const ;
    void setAvailable(bool);
    void editingFinished();
    bool isErrorState();


private:
    void updateConnectionsTo(QString to);

private:
    Application *app;
    BuilderItem *parent;
    bool portAvailable;
    QGraphicsProxyWidget *lineEditWidget;
    bool errorState;


protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif // DESTINATIONPORTITEM_H
