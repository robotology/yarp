/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "entitiestreewidget.h"
#include <yarp/manager/ymm-dir.h>
#include <QProcess>
#include <QHeaderView>
#include <QMessageBox>
#include <QTreeWidgetItem>

EntitiesTreeWidget::EntitiesTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

    applicationNode = new QTreeWidgetItem(this,QStringList() << "Application");
    modulesNode = new QTreeWidgetItem(this,QStringList() << "Modules");
    resourcesNode = new QTreeWidgetItem(this,QStringList() << "Resources");
    templatesNode = new QTreeWidgetItem(this,QStringList() << "Templates");

    applicationNode->setIcon(0,QIcon(":/folder-app.svg"));
    modulesNode->setIcon(0,QIcon(":/folder-mod.svg"));
    resourcesNode->setIcon(0,QIcon(":/folder-res.svg"));
    templatesNode->setIcon(0,QIcon(":/folder.svg"));

    addTopLevelItem(applicationNode);
    addTopLevelItem(modulesNode);
    addTopLevelItem(resourcesNode);
    addTopLevelItem(templatesNode);

    setExpandsOnDoubleClick(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    resizeColumnToContents(0);

    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onContext(QPoint)));

    openFile = new QAction("Open File",this);
    importFile = new QAction("Import Files...",this);

    topLevelMenu.addAction(openFile);
    topLevelMenu.addAction(importFile);

    loadFiles = new QAction("Load",this);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    reopen = new QAction("Refresh",this);
    remove = new QAction("Remove",this);

    secondLevelMenu.addAction(loadFiles);
    secondLevelMenu.addAction(separator);
    secondLevelMenu.addAction(reopen);
    secondLevelMenu.addAction(remove);

    edit = new QAction("Edit",this);
    leafLevelMenu.addAction(edit);

    connect(loadFiles,SIGNAL(triggered()),this,SLOT(onLoadFile()));
    connect(openFile,SIGNAL(triggered()),this,SIGNAL(openFiles()));
    connect(importFile,SIGNAL(triggered()),this,SIGNAL(importFiles()));
    connect(edit,SIGNAL(triggered()),this,SLOT(onEdit()));
    connect(remove,SIGNAL(triggered()),this,SLOT(onRemove()));
    connect(reopen,SIGNAL(triggered()),this,SLOT(onReopen()));

    #if defined(WIN32)
        ext_editor = "notepad.exe";
    #else
        ext_editor = "xdg-open";
    #endif
}

void EntitiesTreeWidget::setExtEditor(string editor)
{
    ext_editor = editor.c_str();
}

/*! \brief Add an application to the tree
    \param app the application
*/
void EntitiesTreeWidget::addApplication(yarp::manager::Application *app)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(applicationNode,QStringList() << app->getName());
    item->setData(0,Qt::UserRole + 1,qlonglong(app));
    item->setData(0,Qt::UserRole, yarp::manager::APPLICATION);
    item->setIcon(0,QIcon(":/run22.svg"));



    string fname;
    string fpath = app->getXmlFile();
    size_t pos = fpath.rfind(PATH_SEPERATOR);
    if(pos!=string::npos){
        fname = fpath.substr(pos+1);
    } else {
        fname = fpath;
    }
    fname = fname + string(" (") + fpath + string(")");

    QTreeWidgetItem *xml = new QTreeWidgetItem(item,QStringList() << fname.data());
    xml->setData(0,Qt::UserRole + 1,QString(fpath.data()));
    xml->setData(0,Qt::UserRole, yarp::manager::NODE_FILENAME);
    xml->setIcon(0,QIcon(":/file-xml22.svg"));
}

/*! \brief Add a resource to the tree
    \param comp the resource
*/
void EntitiesTreeWidget::addComputer(yarp::manager::Computer* comp)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(resourcesNode,QStringList() << comp->getName());
    item->setData(0,Qt::UserRole + 1,qlonglong(comp));
    item->setData(0,Qt::UserRole, yarp::manager::RESOURCE);
    item->setIcon(0,QIcon(":/computer22.svg"));

    string fname;
    string fpath = comp->getXmlFile();
    size_t pos = fpath.rfind(PATH_SEPERATOR);
    if(pos!=string::npos){
        fname = fpath.substr(pos+1);
    } else {
        fname = fpath;
    }
    fname = fname + string(" (") + fpath + string(")");

    QTreeWidgetItem *xml = new QTreeWidgetItem(item,QStringList() << fname.data());
    xml->setData(0,Qt::UserRole + 1,QString(fpath.data()));
    xml->setData(0,Qt::UserRole, yarp::manager::NODE_FILENAME);
}

/*! \brief Add a module to the tree
    \param mod the module
*/
void EntitiesTreeWidget::addModule(yarp::manager::Module* mod)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(modulesNode,QStringList() << mod->getName());
    item->setData(0,Qt::UserRole + 1,qlonglong(mod));
    item->setData(0,Qt::UserRole, yarp::manager::MODULE);
    item->setIcon(0,QIcon(":/module22.svg"));

    string fname;
    string fpath = mod->getXmlFile();
    size_t pos = fpath.rfind(PATH_SEPERATOR);
    if(pos!=string::npos){
        fname = fpath.substr(pos+1);
    } else {
        fname = fpath;
    }
    fname = fname + string(" (") + fpath + string(")");

    QTreeWidgetItem *xml = new QTreeWidgetItem(item,QStringList() << fname.data());
    xml->setData(0,Qt::UserRole + 1,QString(fpath.data()));
    xml->setData(0,Qt::UserRole, yarp::manager::NODE_FILENAME);
}

/*! \brief Add an application template to the tree
    \param tmp the application template
*/
void EntitiesTreeWidget::addAppTemplate(yarp::manager::AppTemplate* tmp)
{

    QTreeWidgetItem *item = new QTreeWidgetItem(templatesNode,QStringList() << QString("%1 (%2)").arg(tmp->name.data()).arg(tmp->tmpFileName.data()));

    item->setData(0,Qt::UserRole, yarp::manager::NODE_APPTEMPLATE);
    item->setData(0,Qt::UserRole + 1 ,tmp->name.data());
    item->setData(0,Qt::UserRole + 2 ,tmp->tmpFileName.data());
    item->setIcon(0,QIcon(":/file-xml22.svg"));
}

/*! \brief Called when an item has been double clicked

    when an elemtn of the entities tree has been double clicked it will open its relative view
    in the main window

    \param item the item has been double clicked
    \param column the column has been double clicked
*/
void EntitiesTreeWidget::onItemDoubleClicked(QTreeWidgetItem *item,int column)
{
    Q_UNUSED(column);

    if(item->data(0,Qt::UserRole)  == yarp::manager::APPLICATION){
        yarp::manager::Application *app = (yarp::manager::Application*)item->data(0,Qt::UserRole + 1).toLongLong();
        viewApplication(app);
    }

    if(item->data(0,Qt::UserRole)  == yarp::manager::MODULE){
        yarp::manager::Module *mod = (yarp::manager::Module*)item->data(0,Qt::UserRole + 1).toLongLong();
        viewModule(mod);
    }

    if(item->data(0,Qt::UserRole)  == yarp::manager::RESOURCE){
        yarp::manager::Computer *res = (yarp::manager::Computer*)item->data(0,Qt::UserRole + 1).toLongLong();
        viewResource(res);
    }

    if(item->data(0,Qt::UserRole)  == yarp::manager::NODE_APPTEMPLATE){
        QString name = item->data(0,Qt::UserRole + 1).toString();
        QString tmpFileName = item->data(0,Qt::UserRole + 2).toString();
        qDebug("%s",name.toLatin1().data());

        QProcess *notepad;
        notepad = new QProcess(this);
        notepad->start(ext_editor,QStringList()<<tmpFileName);

    }

    if(item->data(0,Qt::UserRole)  == yarp::manager::NODE_FILENAME){
        QString fileName = item->data(0,Qt::UserRole + 1).toString();
        qDebug("%s",fileName.toLatin1().data());

        QProcess *notepad;
        notepad = new QProcess(this);
        notepad->start(ext_editor,QStringList()<<fileName);
    }
}


/*! \brief Clear the application node
*/
void EntitiesTreeWidget::clearApplication()
{
    if(!applicationNode){
        return;
    }
    while(applicationNode->childCount() > 0){
        applicationNode->removeChild(applicationNode->child(0));
    }
}

/*! \brief Clear the module node
*/
void EntitiesTreeWidget::clearModules()
{
    if(!modulesNode){
        return;
    }
    while(modulesNode->childCount() > 0){
        modulesNode->removeChild(modulesNode->child(0));
    }
}

/*! \brief Clear the resource node
*/
void EntitiesTreeWidget::clearResources()
{
    if(!resourcesNode){
        return;
    }
    while(resourcesNode->childCount() > 0){
        resourcesNode->removeChild(resourcesNode->child(0));
    }
}

/*! \brief Clear the application template node
*/
void EntitiesTreeWidget::clearTemplates()
{
    if(!templatesNode){
        return;
    }
    while(templatesNode->childCount() > 0){
        templatesNode->removeChild(templatesNode->child(0));
    }
}

/*! \brief Called when a context menu has been requested
    \param p the point where the context menu should appear
*/
void EntitiesTreeWidget::onContext(QPoint p)
{
    QTreeWidgetItem *it = itemAt(p);

    if(!it){
        return;
    }
    QPoint pp = QPoint(p.x(),p.y() + header()->height());
    if(it == applicationNode || it ==resourcesNode || it == modulesNode || it == templatesNode){
        topLevelMenu.exec(mapToGlobal(pp));
    }else{
        if(it->parent() == applicationNode){
            loadFiles->setText("Load Application");
            secondLevelMenu.exec(mapToGlobal(pp));
        }else
        if(it->parent() == resourcesNode){
            loadFiles->setText("Load Resource");
            secondLevelMenu.exec(mapToGlobal(pp));
        }else
        if(it->parent() == modulesNode){
            loadFiles->setText("Load Module");
            secondLevelMenu.exec(mapToGlobal(pp));
        }else{
            leafLevelMenu.exec(mapToGlobal(pp));
        }
    }
}

/*! \brief Called when a file has been loaded.
 *
 *  It's the same as double click an item
*/
void EntitiesTreeWidget::onLoadFile()
{
    QTreeWidgetItem *it = currentItem();

    if(!it){
        return;
    }

    if(it->parent() == applicationNode){
        if(it->data(0,Qt::UserRole)  == yarp::manager::APPLICATION){
            yarp::manager::Application *app = (yarp::manager::Application*)it->data(0,Qt::UserRole + 1).toLongLong();
            viewApplication(app);
        }
    }else
    if(it->parent() == resourcesNode){
        if(it->data(0,Qt::UserRole)  == yarp::manager::RESOURCE){
            yarp::manager::Computer *res = (yarp::manager::Computer*)it->data(0,Qt::UserRole + 1).toLongLong();
            viewResource(res);
        }
    }else
    if(it->parent() == modulesNode){
        if(it->data(0,Qt::UserRole)  == yarp::manager::MODULE){
            yarp::manager::Module *mod = (yarp::manager::Module*)it->data(0,Qt::UserRole + 1).toLongLong();
            viewModule(mod);
        }
    }

}

/*! \brief Edit an item.
 *
 * Edit only leafs element of the tree that are xml representing applications, modules, or templates
*/
void EntitiesTreeWidget::onEdit()
{
    QTreeWidgetItem *item = currentItem();

    if(!item){
        return;
    }
    if(item->data(0,Qt::UserRole)  == yarp::manager::NODE_FILENAME){
        QString fileName = item->data(0,Qt::UserRole + 1).toString();

        QProcess *notepad;
        notepad = new QProcess(this);
        notepad->start(ext_editor,QStringList()<<fileName);
    }else if(item->data(0,Qt::UserRole)  == yarp::manager::NODE_APPTEMPLATE){
            QString name = item->data(0,Qt::UserRole + 1).toString();
            QString tmpFileName = item->data(0,Qt::UserRole + 2).toString();
            qDebug("%s",name.toLatin1().data());

            QProcess *notepad;
            notepad = new QProcess(this);
            notepad->start(ext_editor,QStringList()<<tmpFileName);

    }

}

/*! \brief Reload the selected entity node
*/
void EntitiesTreeWidget::onReopen()
{
    QTreeWidgetItem *it = currentItem();
    QTreeWidgetItem *parent = it -> parent();
    int index = it -> parent() -> indexOfChild(it);

    if(!it){
        return;
    }

    if(it->parent() == applicationNode){
        if(it->data(0,Qt::UserRole)  == yarp::manager::APPLICATION){
            yarp::manager::Application *app = (yarp::manager::Application*)it->data(0,Qt::UserRole + 1).toLongLong();
            if(app){
                QString fileName = QString("%1").arg(app->getXmlFile());
                QString appName = it->text(0);

                reopenApplication(appName,fileName);
            }

        }
    }else
    if(it->parent() == resourcesNode){
        if(it->data(0,Qt::UserRole)  == yarp::manager::RESOURCE){
            yarp::manager::Computer *res = (yarp::manager::Computer*)it->data(0,Qt::UserRole + 1).toLongLong();
            if(res){
                QString fileName = QString("%1").arg(res->getXmlFile());
                QString resName = it->text(0);

                reopenResource(resName,fileName);
            }
        }
    }else
    if(it->parent() == modulesNode){
        if(it->data(0,Qt::UserRole)  == yarp::manager::MODULE){
            yarp::manager::Module *mod = (yarp::manager::Module*)it->data(0,Qt::UserRole + 1).toLongLong();
            if(mod){
                QString fileName = QString("%1").arg(mod->getXmlFile());
                QString modName = it->text(0);

                reopenModule(modName,fileName);
            }
        }
    }

    parent -> child(index) -> setSelected(true);
    scrollToItem(parent -> child(index));
}

/*! \brief Remove the selected entity node
*/
void EntitiesTreeWidget::onRemove()
{
    QTreeWidgetItem *item = currentItem();


    if(!item){
        return;
    }



    if(QMessageBox::question(this,"Removing","Are you sure to remove this item?") == QMessageBox::Yes){

        if(item->parent() == applicationNode){
            if(item->data(0,Qt::UserRole)  == yarp::manager::APPLICATION){
                yarp::manager::Application *app = (yarp::manager::Application*)item->data(0,Qt::UserRole + 1).toLongLong();
                if(app){
                    QString appName = item->text(0);

                    removeApplication(appName);
                }

            }
        }else
            if(item->parent() == resourcesNode){
                if(item->data(0,Qt::UserRole)  == yarp::manager::RESOURCE){
                    yarp::manager::Computer *res = (yarp::manager::Computer*)item->data(0,Qt::UserRole + 1).toLongLong();
                    if(res){
                        QString resName = item->text(0);

                        removeResource(resName);
                    }
                }
            }else
            if(item->parent() == modulesNode){
                if(item->data(0,Qt::UserRole)  == yarp::manager::MODULE){
                    yarp::manager::Module *mod = (yarp::manager::Module*)item->data(0,Qt::UserRole + 1).toLongLong();
                    if(mod){
                        QString modName = item->text(0);

                        removeModule(modName);
                    }
                }
            }

            while(item->childCount()>0){
                delete item->takeChild(0);
            }

            if(item->parent()){
                int index = item->parent()->indexOfChild(item);
                delete item->parent()->takeChild(index);
            }
    }
}
