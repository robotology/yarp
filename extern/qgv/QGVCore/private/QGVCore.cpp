/***************************************************************
QGVCore
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
#include "QGVCore.h"
#include <QDebug>

qreal QGVCore::graphHeight(Agraph_t *graph)
{
    //Hauteur totale du graphique (permet d'effectuer le calcul inverse des coordonn�es)
    return GD_bb(graph).UR.y;
}

QPointF QGVCore::toPoint(pointf p, qreal gheight)
{
    //Le repere Y commence du bas dans graphViz et du haut pour Qt !
    return QPointF(p.x, gheight - p.y);
}

QPointF QGVCore::toPoint(point p, qreal gheight)
{
    //Le repere Y commence du bas dans graphViz et du haut pour Qt !
    return QPointF(p.x, gheight - p.y);
}

QPointF QGVCore::centerToOrigin(const QPointF &p, qreal width, qreal height)
{
    //L'origine d'un objet est le centre dans graphViz et du haut gauche pour Qt !
    return QPointF(p.x() - width/2, p.y() - height/2);
}

QPolygonF QGVCore::toPolygon(const polygon_t *poly, qreal width, qreal height)
{
    if (poly->peripheries != 1)
        qWarning("unsupported number of peripheries %d", poly->peripheries);

    const int sides = poly->sides;
    const pointf* vertices = poly->vertices;

    QPolygonF polygon;
    for (int side = 0; side < sides; side++)
        polygon.append(QPointF(vertices[side].x + width/2, vertices[side].y + height/2));
    return polygon;
}

QPainterPath QGVCore::toPath(const char *type, const polygon_t *poly, qreal width, qreal height)
{
    QPainterPath path;
    if ((strcmp(type, "rectangle") == 0) ||
        (strcmp(type, "box") == 0) ||
        (strcmp(type, "hexagon") == 0) ||
        (strcmp(type, "polygon") == 0) ||
        (strcmp(type, "diamond") == 0))
    {
        QPolygonF polygon = toPolygon(poly, width, height);
        polygon.append(polygon[0]);
        path.addPolygon(polygon);
    }
    else if ((strcmp(type, "ellipse") == 0) ||
            (strcmp(type, "circle") == 0))
    {
        QPolygonF polygon = toPolygon(poly, width, height);
        path.addEllipse(QRectF(polygon[0], polygon[1]));
    }
    else
    {
        qWarning("unsupported shape %s", type);
    }
    return path;
}

QPainterPath QGVCore::toPath(const splines *spl, qreal gheight)
{
    QPainterPath path;
    if((spl->list != 0) && (spl->list->size%3 == 1))
    {
        bezier bez = spl->list[0];
        //If there is a starting point, draw a line from it to the first curve point
        if(bez.sflag)
        {
            path.moveTo(toPoint(bez.sp, gheight));
            path.lineTo(toPoint(bez.list[0], gheight));
        }
        else
            path.moveTo(toPoint(bez.list[0], gheight));

        //Loop over the curve points
        for(int i=1; i<bez.size; i+=3)
            path.cubicTo(toPoint(bez.list[i], gheight), toPoint(bez.list[i+1], gheight), toPoint(bez.list[i+2], gheight));

        //If there is an ending point, draw a line to it
        if(bez.eflag)
            path.lineTo(toPoint(bez.ep, gheight));
    }
    return path;
}

Qt::BrushStyle QGVCore::toBrushStyle(const QString &style)
{
    if(style == "filled")
        return Qt::SolidPattern;
    return Qt::NoBrush;
}

Qt::PenStyle QGVCore::toPenStyle(const QString &style)
{
    if(style =="dashed")
        return Qt::DashLine;
    else if(style == "dotted")
        return Qt::DotLine;
    return Qt::SolidLine;
}

QColor QGVCore::toColor(const QString &color)
{
    return QColor(color);
}
