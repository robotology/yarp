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
#include <customtreewidget.h>

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
                                   QWidget *parent = nullptr);
    ~ApplicationViewWidget() override;

    void runApplicationSet(bool onlySelected);
    void stopApplicationSet(bool onlySelected);
    void killApplicationSet(bool onlySelected);
    void connectConnectionSet(bool onlySelected);
    void disconnectConnectionSet(bool onlySelected);
    void refresh();
    void selectAll();
    void exportGraph();

    bool isRunning();

    void onModStart(int which) override;
    void onModStop(int which) override;
    void onModStdout(int which, const char* msg) override;
    void onConConnect(int which) override;
    void onConDisconnect(int which) override;
    void onResAvailable(int which) override;
    void onResUnAvailable(int which) override;
    void onConAvailable(int from, int to) override;
    void onConUnAvailable(int from, int to) override;
    void onError() override;
    void onLoadBalance() override;
    void closeManager();
    QToolBar* getBuilderToolBar();
    bool isBuilderFloating();
    void showBuilder(bool);

    bool anyModuleSelected();
    bool anyConnectionSelected();

    CustomTreeWidget* getModuleList();
    CustomTreeWidget* getConnectionList();

    bool save();
    QString getFileName();
    void setFileName(QString filename);
    QString getAppName();
    void setAppName(QString appName);

    bool isEditingMode();

private:
    bool getConRowByID(int id, int *row);
    bool getResRowByID(int id, int *row);
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
    bool scanAvailableCarriers(QString carrier, bool isConnection = true);
    void updateConnection(int index, std::vector<int> &CIDs);



private:
    QMainWindow *builderWindowContainer;
    QDockWidget *builderWidget;
    BuilderWindow *builder;
    QToolBar *builderToolBar;
    QStringList stringLst;

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

    std::vector<std::string> listOfResourceNames;


private slots:
    void onAssignHost();
    void onCloseStdOut(int);
    void onAttachStdout();
    void onDetachStdout();
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

    void onModuleItemChanged(QTreeWidgetItem*,int);

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


};

#endif // APPLICATIONVIEWWIDGET_H
