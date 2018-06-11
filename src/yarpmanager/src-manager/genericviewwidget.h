/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
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
