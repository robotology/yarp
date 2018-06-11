/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef RESOURCEVIEWWIDGET_H
#define RESOURCEVIEWWIDGET_H
#include <yarp/manager/manager.h>
#include <QWidget>
#include "genericviewwidget.h"
#include <QTreeWidgetItem>

namespace Ui {
class ResourceViewWidget;
}

/*! \class ResourceViewWidget
    \brief The Resource View Widget
*/
class ResourceViewWidget : public GenericViewWidget
{
    Q_OBJECT

public:
    explicit ResourceViewWidget(yarp::manager::Computer *res,QWidget *parent = 0);
    ~ResourceViewWidget();
    void refresh();

private:
    void init();

private:
    yarp::manager::Computer *res;
    Ui::ResourceViewWidget *ui;
};

#endif // RESOURCEVIEWWIDGET_H
