/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ENTITIESTREEWIDGET_H
#define ENTITIESTREEWIDGET_H

#include <QTreeWidget>
#include <QAction>
#include <QMenu>
#include <yarp/manager/manager.h>

#if defined(WIN32)
#define TEXTEDITOR "notepad.exe"
#else
#define TEXTEDITOR "gedit"
#endif


/*! \class EntitiesTreeWidget
    \brief The Entities QtreeWidget
*/
class EntitiesTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit EntitiesTreeWidget(QWidget *parent = 0);

    void addApplication(yarp::manager::Application *app);
    void addComputer(yarp::manager::Computer* comp);
    void addModule(yarp::manager::Module* mod);
    void addAppTemplate(yarp::manager::AppTemplate* tmp);

    void clearApplication();
    void clearModules();
    void clearResources();
    void clearTemplates();

private:
    QTreeWidgetItem *applicationNode;
    QTreeWidgetItem *modulesNode;
    QTreeWidgetItem *resourcesNode;
    QTreeWidgetItem *templatesNode;

    QMenu topLevelMenu;
    QMenu secondLevelMenu;
    QMenu leafLevelMenu;

    QAction *openFile;
    QAction *importFile;
    QAction *loadFiles;
    QAction *reopen;
    QAction *remove;
    QAction *edit;


signals:
    void viewResource(yarp::manager::Computer*);
    void viewModule(yarp::manager::Module*);
    void viewApplication(yarp::manager::Application*);
    void importFiles();
    void openFiles();
    void removeApplication(QString);
    void removeModule(QString);
    void removeResource(QString);
    void reopenApplication(QString, QString);
    void reopenResource(QString, QString);
    void reopenModule(QString, QString);
public slots:
    void onContext(QPoint);
    void onLoadFile();
    void onEdit();
    void onRemove();
    void onReopen();
    void onItemDoubleClicked(QTreeWidgetItem*item, int column);

};

#endif // ENTITIESTREEWIDGET_H
