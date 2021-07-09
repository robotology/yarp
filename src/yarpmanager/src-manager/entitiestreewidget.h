/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
    void addPort(QStringList portDetails);

    void clearApplications();
    void clearModules();
    void clearResources();
    void clearTemplates();
    void clearPorts();

    QTreeWidgetItem * getWidgetItemByFilename(const QString xmlFile);

    void setExtEditor(std::string editor);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QTreeWidgetItem *applicationNode;
    QTreeWidgetItem *modulesNode;
    QTreeWidgetItem *resourcesNode;
    QTreeWidgetItem *templatesNode;
    QTreeWidgetItem *portsNode;

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

    bool missingFile;


signals:
    void viewResource(yarp::manager::Computer*);
    void viewModule(yarp::manager::Module*);
    void viewApplication(yarp::manager::Application*, bool editing = false);
    void importFiles();
    void openFiles();
    void removeApplication(QString,QString);
    void removeModule(QString);
    void removeResource(QString);
    void reopenApplication(QString, QString);
    void reopenResource(QString, QString);
    void reopenModule(QString, QString);
public slots:
    void onSelectItem(QString, bool open = false);
    void onContext(QPoint);
    void onLoadFile();
    void onEditApplication();
    void onEdit();
    void onRemove();
    void onReopen();
    void onItemDoubleClicked(QTreeWidgetItem*item, int column);

};

#endif // ENTITIESTREEWIDGET_H
