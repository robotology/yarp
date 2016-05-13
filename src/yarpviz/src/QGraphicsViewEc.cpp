/***************************************************************
QGVCore Sample
Copyright (c) 2014, Bergont Nicolas, All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
***************************************************************/
#include "QGraphicsViewEc.h"
#include "moc_QGraphicsViewEc.cpp"
#include <QWheelEvent>
#include <qmath.h>

QGraphicsViewEc::QGraphicsViewEc(QWidget* parent) : QGraphicsView(parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}


void QGraphicsViewEc::wheelEvent(QWheelEvent* event)
{
    qreal scaleFactor = qPow(2.0, event->delta() / 240.0); //How fast we zoom
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(0.05 < factor && factor < 10) //Zoom factor limitation
        scale(scaleFactor, scaleFactor);
}
