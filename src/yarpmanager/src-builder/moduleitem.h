#ifndef MODULEITEM_H
#define MODULEITEM_H


#include <QObject>
#include <QPainter>
#include <QGraphicsPolygonItem>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "commons.h"
#include "itemsignalhandler.h"
#include "arrow.h"
#include "builderitem.h"
#include <yarp/manager/manager.h>

class PortItem;
class ItemSignalHandler;
class Arrow;



class ModuleItem : public BuilderItem
{

    friend class Arrow;
    friend class PortItem;

public:
   // ModuleItem(QString itemName, QStringList inputPorts, QStringList outputPorts , BuilderItem * parent = 0);
     ModuleItem(Module *module, int moduleId, bool nestedInApp = false, bool editingMode = false,
                Manager *manager = NULL, BuilderItem * parent = 0);
    ~ModuleItem();
    QRectF boundingRect() const;
    QPointF connectionPoint();
    int getId();
    void setRunning(bool);
    void setModuleSelected(bool selected);
    int type() const ;
    void updateGraphicModel();


    Module *getInnerModule();

    QList <PortItem*> iPorts;
    QList <PortItem*> oPorts;

private:
    void init();


private:
    Manager *manager;
    bool externalSelection;
    bool editingMode;
    int moduleId;
    Module *module;
    QStringList inputPorts;
    QStringList outputPorts;
    bool running;




protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void portPressed(PortItem *port, QGraphicsSceneMouseEvent *e);
    void portReleased(PortItem *port, QGraphicsSceneMouseEvent *e);
    void portMoved(PortItem *port, QGraphicsSceneMouseEvent *e);

signals:
    //void moduleSelected(int);

};

class PortItem : public BuilderItem
{
    friend class Arrow;

public:
    enum PortStatus {unknown,availbale,unavailable} ;
    //PortItem(QString portName, int type, BuilderItem *parent = 0);
    PortItem(InputData*, BuilderItem *parent = 0);
    PortItem(OutputData*, BuilderItem *parent = 0);
    QRectF boundingRect() const;
    QPointF connectionPoint();
    int type() const ;
    int getPortType();
    InputData *getInputData();
    OutputData *getOutputData();
    void setAvailable(PortStatus);

    OutputData *outData;
    InputData *inData;
protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    PortStatus portAvailable;
    BuilderItem *parent;
    QPolygonF polygon;
    int portType;
    qreal triangleH;
    bool hovered;
};

#endif // MODULEITEM_H
