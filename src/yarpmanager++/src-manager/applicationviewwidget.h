/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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
#include <yarp/manager/module.h>
#include <QTreeWidgetItem>
#include "safe_manager.h"
#include "genericviewwidget.h"
#include "stdoutwindow.h"
#include <QList>
#include <QDockWidget>
#include "yarpbuilderlib.h"

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
    explicit ApplicationViewWidget(yarp::manager::Application *,
                                   yarp::manager::Manager *lazyManager,
                                   yarp::os::Property* config,
                                   bool editingMode = false,
                                   QWidget *parent = 0);
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
    void closeManager();
    QToolBar* getBuilderToolBar();
    bool isBuilderFloating();
    void showBuilder(bool);

    bool save();
    QString getFileName();
    QString getAppName();

    bool isEditingMode();


private:
    bool getConRowByID(int id, int *row);
    QTreeWidgetItem *getModRowByID(int id, QTreeWidgetItem *parent = NULL);
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
    bool areAllShutdown();
    void runNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs);
    void stopNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs);
    void killNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs);
    void refreshNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs);
    bool areAllNestedApplicationShutdown(QTreeWidgetItem *it);
    void closeNestedApplicationStdOut(QTreeWidgetItem *it,int id);
    void assignHostNestedApplication(QTreeWidgetItem *it);
    void attachStdOutNestedApplication(QTreeWidgetItem *it,std::vector<int> *MIDs);
    void modStdOutNestedApplication(QTreeWidgetItem *it, int id,QString s);
    void selectAllNestedApplicationModule(QTreeWidgetItem *it, bool check);


private:
    QMainWindow *builderWindowContainer;
    QDockWidget *builderWidget;
    BuilderWindow *builder;
    QToolBar *builderToolBar;

    Ui::ApplicationViewWidget *ui;
    SafeManager safeManager;
    yarp::manager::Manager *lazy;
    yarp::manager::Application *app;
    yarp::os::Property* m_pConfig;
    yarp::manager::Manager man;

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

    QList <StdoutWindow*> stdoutWinList;

    bool editingMode;


private slots:
    void onAssignHost();
    void onCloseStdOut(int);
    void onAttachStdout(void);
    void onDetachStdout(void);
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

    void onSelfSafeLoadBalance();
    void onSelfConnect(int which);
    void onSelfDisconnect(int which);
    void onSelfResAvailable(int which);
    void onSelfResUnavailable(int which);
    void onSelfStart(int which);
    void onSelfStop(int which);

    void onRefreshApplication();
    void onModuleSelected(QList<int> ids);
    void onConnectionSelected(QList<int> id);
    //void onBuilderFloatingChanged(bool);

    void onModuleItemChanged(QTreeWidgetItem*,int);
    void onBuilderFloatChanged(bool);

signals:
    void logWarning(QString);
    void logError(QString);

    void selfSafeLoadBolance();
    void selfConnect(int);
    void selfDisconnect(int);
    void selfResAvailable(int);
    void selfResUnavailable(int);
    void selfStart(int);
    void selfStop(int);
    void builderWindowFloating(bool);



};

#endif // APPLICATIONVIEWWIDGET_H
