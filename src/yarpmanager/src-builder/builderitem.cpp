/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "builderitem.h"
#include "arrow.h"
#include <cmath>

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

QList<Arrow *>* BuilderItem::getArrows()
{
    return &arrows;
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
    return {xV, yV};
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
