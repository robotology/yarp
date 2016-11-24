/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

    void setExtEditor(string editor);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

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
    QAction *editApplication;
    QAction *reopen;
    QAction *remove;
    QAction *edit;

    QString ext_editor;


signals:
    void viewResource(yarp::manager::Computer*);
    void viewModule(yarp::manager::Module*);
    void viewApplication(yarp::manager::Application*, bool editing = false);
    void importFiles();
    void openFiles();
    void removeApplication(QString);
    void removeModule(QString);
    void removeResource(QString);
    void reopenApplication(QString, QString);
    void reopenResource(QString, QString);
    void reopenModule(QString, QString);
public slots:
    void onSelectItem(QString);
    void onContext(QPoint);
    void onLoadFile();
    void onEditApplication();
    void onEdit();
    void onRemove();
    void onReopen();
    void onItemDoubleClicked(QTreeWidgetItem*item, int column);

};

#endif // ENTITIESTREEWIDGET_H
