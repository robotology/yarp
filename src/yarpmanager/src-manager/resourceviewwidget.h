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
