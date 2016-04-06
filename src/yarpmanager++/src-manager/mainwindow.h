/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>
#include <yarp/manager/manager.h>
#include "entitiestreewidget.h"
#include "genericviewwidget.h"
#include "newapplicationwizard.h"
//#include "message_list.h"
//#include "application_list.h"

namespace Ui {
class MainWindow;
}

/*! \class MainWindow
    \brief MainWindow class.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class NewApplicationWizard;

public:
    explicit MainWindow(QWidget *parent = 0);
    void init(yarp::os::Property config);
    ~MainWindow();

    void reportErrors();

private:
    void syncApplicationList(QString selectNodeForEditing = "");
    bool loadRecursiveTemplates(const char* szPath);
    bool loadRecursiveApplications(const char* szPath);

private:
    Ui::MainWindow *ui;
    yarp::manager::Manager lazyManager;
    yarp::os::Property config;

    EntitiesTreeWidget *entitiesTree;
    QToolBar *builderToolBar;
    GenericViewWidget *prevWidget;

    string ext_editor;

protected:
    void closeEvent(QCloseEvent *);


private slots:
    void onSave();
    void onOpen();
    void onClose();
    void onImportFiles();
    void onNewModule();
    void onNewResource();
    void onNewApplication();
    void onExportGraph();
    void onRun();
    void onStop();
    void onKill();
    void onConnect();
    void onDisconnect();
    void onRefresh();
    void onSelectAll();
    bool onTabClose(int);
    void onLogError(QString);
    void onLogWarning(QString);
    void onLogMessage(QString);
    void onHelp();
    void onAbout();
    void onBuilderWindowFloating(bool);
    void onWizardError(QString);

    void onModified(bool);

public slots:
    void onTabChangeItem(int);
    void viewModule(yarp::manager::Module*);
    void viewResource(yarp::manager::Computer *res);
    void viewApplication(yarp::manager::Application *app, bool editingMode);

    void onRemoveApplication(QString);
    void onRemoveModule(QString);
    void onRemoveResource(QString);
    void onReopenApplication(QString,QString);
    void onReopenModule(QString,QString);
    void onReopenResource(QString,QString);

signals:
    void selectItem(QString);
};

#endif // MAINWINDOW_H
