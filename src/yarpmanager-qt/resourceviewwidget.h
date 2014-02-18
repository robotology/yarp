/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef RESOURCEVIEWWIDGET_H
#define RESOURCEVIEWWIDGET_H
#include <yarp/manager/manager.h>
#include <QWidget>
#include "genericviewwidget.h"


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

private:
    yarp::manager::Computer *res;
    Ui::ResourceViewWidget *ui;
};

#endif // RESOURCEVIEWWIDGET_H
