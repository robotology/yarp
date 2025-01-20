/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
