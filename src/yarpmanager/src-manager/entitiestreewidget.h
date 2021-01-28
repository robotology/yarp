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
