#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>
#include "manager.h"
#include "entitiestreewidget.h"
//#include "message_list.h"
//#include "application_list.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void init(yarp::os::Property config);
    ~MainWindow();

    void reportErrors();

private:
    void syncApplicationList();
    bool loadRecursiveTemplates(const char* szPath);
    bool loadRecursiveApplications(const char* szPath);


private:
    Ui::MainWindow *ui;
    Manager lazyManager;
    yarp::os::Property config;

    EntitiesTreeWidget *entitiesTree;

private slots:
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
    void onTabClose(int);
    void onLogError(QString);
    void onLogWarning(QString);
    void onLogMessage(QString);

public slots:
    void onTabChangeItem(int);
    void viewModule(Module*);
    void viewResource(Computer *res);
    void viewApplication(Application *app);
};

#endif // MAINWINDOW_H
