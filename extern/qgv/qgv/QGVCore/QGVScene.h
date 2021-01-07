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
#ifndef QGVSCENE_H
#define QGVSCENE_H

#include "qgv.h"
#include <QGraphicsScene>

class QGVNode;
class QGVEdge;
class QGVSubGraph;

class QGVGraphPrivate;
class QGVGvcPrivate;

/**
 * @brief GraphViz interactive scene
 *
 */
class QGVCORE_EXPORT QGVScene : public QGraphicsScene
{
    Q_OBJECT
public:

    explicit QGVScene(const QString &name, QObject *parent = 0, bool gridNeeded = true);
    ~QGVScene();

    void setGraphAttribute(const QString &name, const QString &value);
    void setNodeAttribute(const QString &name, const QString &value);
    void setEdgeAttribute(const QString &name, const QString &value);

    QGVNode* addNode(const QString& label);
    QGVEdge* addEdge(QGVNode* source, QGVNode* target, const QString& label=QString());
    QGVSubGraph* addSubGraph(const QString& name, bool cluster=true);

    void deleteNode(QGVNode *node);
    void deleteEdge(QGVEdge *edge);
    void deleteSubGraph(QGVSubGraph *subgraph);

    void setRootNode(QGVNode *node);

    void loadLayout(const QString &text);
    void applyLayout();
    void clear();

    void enableBgGrid(bool enable);  // Enables or disables the "backgroung grid" added when rendering the scene
    bool bgGridEnabled();


signals:
    void nodeContextMenu(QGVNode* node);
    void nodeDoubleClick(QGVNode* node);

    void edgeContextMenu(QGVEdge* edge);
    void edgeDoubleClick(QGVEdge* edge);

    void subGraphContextMenu(QGVSubGraph* graph);
    void subGraphDoubleClick(QGVSubGraph* graph);

    void graphContextMenuEvent();
    
public slots:

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * contextMenuEvent);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void drawBackground(QPainter * painter, const QRectF & rect);
private:
    friend class QGVNode;
    friend class QGVEdge;
    friend class QGVSubGraph;

    QGVGvcPrivate *_context;
    QGVGraphPrivate *_graph;
    //QFont _font;

    QList<QGVNode*> _nodes;
    QList<QGVEdge*> _edges;
    QList<QGVSubGraph*> _subGraphs;

    bool m_gridNeeded;  // If false, the drawBackground function will not apply the grid to the image background
};

#endif // QGVSCENE_H
