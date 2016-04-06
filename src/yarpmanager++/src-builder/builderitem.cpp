#include "builderitem.h"
#include "arrow.h"
#include "math.h"
ItemSignalHandler *BuilderItem::signalHandler(){
    return sigHandler;
}
void BuilderItem::addArrow(Arrow *arrow){
    arrows.append(arrow);
}
void BuilderItem::removeArrows(){
    foreach (Arrow *arrow, arrows) {
        //arrow->hide();
//        arrow->startItem()->removeArrow(arrow);
//        arrow->endItem()->removeArrow(arrow);
        delete arrow;
    }
}

void BuilderItem::updateConnections(){
    foreach (Arrow *arrow, arrows) {
        if(arrow){
            arrow->updatePosition();
        }
    }
}



void BuilderItem::removeArrow(Arrow *arrow){
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);


}

bool BuilderItem::arrowAlreadyPresent(BuilderItem *endItem)
{
    foreach (Arrow *arrow, arrows) {
        if(arrow->startItem() == this &&
           arrow->endItem() == endItem){
            return true;
        }

    }
    return false;
}

bool BuilderItem::allowInputConnections(){
    return allowInputs;
}
bool BuilderItem::allowOutputConnections(){
    return allowOutputs;
}
void BuilderItem::snapToGrid(bool snap){
    this->snap = snap;
}
QPointF BuilderItem::computeTopLeftGridPoint(const QPointF &pointP){
    int gridSize = 16;
    qreal xV = gridSize/2 + floor(pointP.x()/gridSize)*gridSize;
    qreal yV = gridSize/2 + floor(pointP.y()/gridSize)*gridSize;
    return QPointF(xV, yV);
}

QPointF BuilderItem::getStartingPoint()
{
    return startingPoint;
}
QString BuilderItem::getItemName()
{
    return itemName;
}

QList<QAction*> BuilderItem::getActions()
{
    return actions;
}

void BuilderItem::setActions(QList<QAction*> acts)
{
    actions = acts;
}

bool BuilderItem::isNestedInApp()
{
    return nestedInApp;
}
