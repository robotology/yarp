/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BUILDERWINDOW_H
#define BUILDERWINDOW_H

#include <QWidget>
#include "builderscene.h"
#include <QGraphicsView>
#include "customtree.h"
#include <QGraphicsItem>
#include <QMenu>
#include "commons.h"
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include "builderitem.h"
#include "sourceportitem.h"
#include "destinationportitem.h"
#include "moduleitem.h"
#include "applicationitem.h"
#include "yarpbuilderlib_global.h"
#include <yarp/manager/manager.h>
#include "safe_manager.h"
#include "propertiestable.h"

#include <QToolBar>
#include <QSplitter>

//namespace BuilderUi {
//class BuilderWindow;
//}
// class YARPBUILDERLIBSHARED_EXPORT BuilderWindow : public QWidget

class BuilderWindow : public QWidget
{
    Q_OBJECT

    friend class CustomView;
public:
    explicit BuilderWindow(Application *app,Manager *lazyManager,SafeManager *safeManager,bool editingMode = false,QWidget *parent = 0);


    ~BuilderWindow();
    void setModuleRunning(bool, int id);
    void setConnectionConnected(bool, QString from, QString to);


    void load(bool refresh = false);
    bool save();

    void setSelectedModules(QList<int>selectedIds);
    void setSelectedConnections(QList<int>selectedIds);

    //BuilderItem* addModule(QString itemName, QStringList inputPorts, QStringList outputPorts , QPointF pos, BuilderItem * parent = 0);
    BuilderItem *addModule(Module *module, int moduleId);
    BuilderItem *addSourcePort(QString name, bool editOnStart = false);
    BuilderItem *addDestinantionPort(QString name, bool editOnStart = false);
    BuilderItem *addConnection(void *startItem , void *endItem, int connectionId);
    ApplicationItem* addApplication(Application *app, int *connectionsId = 0);
    void setOutputPortAvailable(QString, bool);
    void setInputPortAvailable(QString, bool);
    QString getFileName();
    void setFileName(QString filename);
    QString getAppName();
    void setAppName(QString appName);

    QToolBar *getToolBar();

    void removeToolBar();
    void addToolBar();
    void addModulesAction(QAction*);
    void addAction(QAction*);
    void addConnectionsAction(QAction*);
    QRectF itemsBoundingRect();

private:
    void prepareManagerFrom(Manager* lazy,
                            const char* szAppName);
    void init();
    bool isApplicationPresent(Application *application);
    bool isModulePresent(Module *module);
    void initModuleTab(ModuleItem *it);
    void findInputOutputData(Connection& cnn,  ModulePContainer &modules,
                             InputData* &input_, OutputData* &output_, QString *inModulePrefix, QString *outModulePrefix);
    PortItem* findModelFromOutput(OutputData* output, QString modulePrefix);
    PortItem* findModelFromInput(InputData* input, QString modulePrefix);


protected:
    //BuilderUi::BuilderWindow *ui;
    QSplitter *splitter;
    QList <int> usedModulesId;
    QToolBar builderToolbar;
    BuilderScene *scene;
    PropertiesTable *propertiesTab;
//    QTreeWidget *appProperties;
//    QTreeWidget *moduleProperties;
//    QTreeWidget *moduleDescription;
    CustomView *view;
    Manager *lazyManager;
    Manager manager;
    SafeManager *safeManager;
    Application *app;
    int index;
    //QList <QGraphicsItem*> itemsList;
    bool editingMode;
    bool m_modified;
    int connectionsId;

    QString editingAppName;
    QString editingAppDescr;
    QString editingAppVersion;
    QString editingAppAuthors;

    QList <QAction*> modulesAction;
    QList <QAction*> builderActions;
    QList <QAction*> connectionsAction;



signals:
    void refreshApplication();
    void setModuleSelected(QList<int>);
    void setConnectionSelected(QList<int>);
    void modified(bool);

private slots:
    void onMoved();
    void onModified();
    void onZoomIn();
    void onZoomOut();
    void onRestoreZoom();
    void onSnap(bool);
    void onShowGrid(bool);
    void onModuleSelected(QGraphicsItem *it);
    void onConnectionSelected(QGraphicsItem *it);
    void onApplicationSelected(QGraphicsItem* it);
    void initApplicationTab();
    BuilderItem *onAddModule(void*, QPointF pos);
    void onAddedApplication(void *app,QPointF pos);
    BuilderItem *onAddNewConnection(void *, void *, int connectionId = -1);
    BuilderItem *onAddSourcePort(QString, QPointF pos);
    BuilderItem * onAddDestinationPort(QString,QPointF pos);

};

class CustomView : public QGraphicsView
{
    Q_OBJECT
    friend class BuilderWindow;
public:
    CustomView(BuilderWindow *builder,QGraphicsView *parent = 0);

    void setEditingMode(bool editing);
    bool getEditingMode();



protected:
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void deleteAllItems();
    void deleteSelectedItems(QGraphicsItem *it = NULL);
    void copySelectedItems();
    void pasteSelectedItems(QPoint pos);
private:
    BuilderItem *pasteItem(QGraphicsItem *item, QSize *offset, bool *firstAdded, QPoint pos);

private:
    BuilderWindow *builder;
    bool editingMode;
    //QPoint origin;
    QPoint m_lastDragPos;
    QPoint m_rubberBandOrigin;
    bool m_rubberBandActive;
    bool mousepressed;
    QRubberBand *rubberBand;
    QList <QGraphicsItem*>copiedItems;

signals:
    void pressedNullItem();
    void addSourcePort(QString,QPointF pos);
    void addDestinationPort(QString,QPointF pos);
    void modified();
    void addModule(void* mod,QPointF);
    void addNewConnection(void *start, void *end);




};


#endif // BUILDERWINDOW_H
