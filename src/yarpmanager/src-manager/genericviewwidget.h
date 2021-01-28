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

#ifndef GENERICVIEWWIDGET_H
#define GENERICVIEWWIDGET_H

#include <QWidget>
#include <yarp/manager/manager.h>
#include "safe_manager.h"

/*! \class GenericViewWidget
    \brief Generic View Widget

    This class represent a generic View Widget that could be a Module View, a Resource View
    or an Application View
*/
class GenericViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GenericViewWidget(QWidget *parent = 0);
    yarp::manager::NodeType getType();
    bool isModified();
    void setModified(bool mod);

protected:
    yarp::manager::NodeType type;
    bool m_modified;
signals:
    void modified(bool);

public slots:
    void onModified(bool mod);

};

#endif // GENERICVIEWWIDGET_H
