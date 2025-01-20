/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include "builderitem.h"

class ApplicationItem : public BuilderItem
{
public:
    ApplicationItem(Application* application, Manager *manager, QList <int> *,
                    bool nestedInApp = false,
                    bool editingMode = false,
                    int *connectionsId = NULL,
                    BuilderItem *parent = NULL);
    ~ApplicationItem();

    QRectF boundingRect() const override;
    QPointF connectionPoint() override;
    int type() const override;
    void init();
    QList<QGraphicsItem *> getModulesList();
    Application* getInnerApplication();
    void setModuleRunning(bool,int);
    void setConnectionConnected(bool connected, QString from, QString to);
    void setSelectedModules(QList<int>selectedIds);
    void setSelectedConnections(QList<int>selectedIds);
    void setOutputPortAvailable(QString oData, bool available);
    void setInputPortAvailable(QString iData, bool available);
    void updateGraphicModel();
    void setModulesAction(QList <QAction*> act);

private:
    void updateBoundingRect(QList<QGraphicsItem *> items);
    void updateSizes(QGraphicsItem *it, QGraphicsItem *parent = NULL);
    void findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                             InputData* &input_, OutputData* &output_);
    PortItem* findModelFromOutput(OutputData* output);
    PortItem* findModelFromInput(InputData* input);

    BuilderItem* addModule(Module *module, int moduleId);
    qreal minx,miny,maxw,maxh;
    int *connectionsId;



protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;


private:
    QList <QAction*> modulesAction;
    bool editingMode;
    QList <int> *usedModulesId;
    QFont customFont;
    int textWidth ;
    Application* application;
    Manager *mainAppManager;
    int index;
    QList <QGraphicsItem*> itemsList;



};

#endif // APPLICATIONITEM_H
