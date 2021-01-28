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

#ifndef BUILDERSCENE_H
#define BUILDERSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <yarp/conf/compiler.h>

class CustomView;
class BuilderWindow;

class BuilderScene : public QGraphicsScene
{
    friend class CustomView;
    friend class BuilderWindow;
    Q_OBJECT
public:
    explicit BuilderScene(QObject *parent = 0);


private:
    QGraphicsLineItem *currentLine;
    QPointF startingPoint;
    QGraphicsItem *startConnectionItem;
    QGraphicsItem *endConnectionItem;

protected:
    bool snap;
    bool editingMode;


protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    //void wheelEvent(QGraphicsSceneWheelEvent *event) override;

signals:
    void addedApplication(void *app,QPointF);
    void addedModule(void *mod,QPointF);
    void addNewConnection(void *start, void *end);

public slots:
    void snapToGrid(bool snap);
    void onNewConnectionRequested(QPointF, QGraphicsItem *item);
    void onNewConnectionAdded(QPointF, QGraphicsItem *item);
    void onSceneChanged(QList<QRectF> rects);

};

#endif // BUILDERSCENE_H
