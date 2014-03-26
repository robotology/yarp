/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef APPLICATIONVIEWWIDGET_H
#define APPLICATIONVIEWWIDGET_H

#include <QWidget>
#include <yarp/manager/manager.h>
#include <QTreeWidgetItem>
#include "safe_manager.h"
#include "genericviewwidget.h"

namespace Ui {
class ApplicationViewWidget;
}

/*! \class ApplicationViewWidget
    \brief The Application View Widget
*/
class ApplicationViewWidget : public GenericViewWidget, public ApplicationEvent
{
    Q_OBJECT

public:
    explicit ApplicationViewWidget(yarp::manager::Application *, yarp::manager::Manager *lazyManager, yarp::os::Property* config,QWidget *parent = 0);
    ~ApplicationViewWidget();

    void runApplicationSet();
    void stopApplicationSet();
    void killApplicationSet();
    void connectConnectionSet();
    void disconnectConnectionSet();
    void refresh();
    void selectAll();
    void exportGraph();

    bool isRunning();

    void onModStart(int which);
    void onModStop(int which);
    void onModStdout(int which, const char* msg);
    void onConConnect(int which);
    void onConDisconnect(int which);
    void onResAvailable(int which);
    void onResUnAvailable(int which);
    void onConAvailable(int from, int to);
    void onConUnAvailable(int from, int to);
    void onError(void);
    void onLoadBalance(void);


private:
    bool getConRowByID(int id, int *row);
    void reportErrors();
    void prepareManagerFrom(yarp::manager::Manager* lazy);
    void updateApplicationWindow();
    bool isEditable(QTreeWidgetItem *it,int col);
    void selectAllModule(bool check);
    void selectAllConnections(bool check);
    void selectAllResources(bool check);
    void createModulesViewContextMenu();
    void createConnectionsViewContextMenu();
    void createResourcesViewContextMenu();
    bool timeout(double base, double timeout);

private:
    Ui::ApplicationViewWidget *ui;
    SafeManager manager;
    yarp::manager::Manager *lazy;
    yarp::manager::Application *app;
    yarp::os::Property* m_pConfig;

    QAction *modRunAction;
    QAction *modStopAction;
    QAction *modkillAction;
    QAction *modSeparator;
    QAction *modRefreshAction;
    QAction *modSelectAllAction;
    QAction *modAttachAction;
    QAction *modAssignAction;

    QMenu *connContex;
    QMenu *connSubMenu;
    QAction *connConnectAction;
    QAction *connDisconnectAction;
    QAction *connSeparatorAction;
    QAction *connRefreshAction;
    QAction *connSelectAllAction;
    QAction *conn1SeparatorAction;
    QAction *connInspectAction;
    QAction *connYARPViewAction;
    QAction *connYARPReadAction;
    QAction *connYARPHearAction;
    QAction *connYARPScopeAction;

    QAction *resRefreshAction;
    QAction *resSelectAllAction;


private slots:
    void onYARPView();
    void onYARPHear();
    void onYARPRead();
    void onYARPScope();
    void onResourceItemSelectionChanged();
    void onConnectionItemSelectionChanged();
    void onModuleItemSelectionChanged();
    void selectAllModule();
    void selectAllConnections();
    void selectAllResources();
    void onItemDoubleClicked(QTreeWidgetItem*,int);
    bool onRun();
    bool onStop();
    bool onKill();
    bool onConnect();
    bool onDisconnect();
    bool onRefresh();

signals:
    void logWarning(QString);
    void logError(QString);

};

#endif // APPLICATIONVIEWWIDGET_H
