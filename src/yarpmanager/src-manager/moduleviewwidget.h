/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * LGPLv2.1+ license. See the accompanying LICENSE file for details.
 */

#ifndef MODULEVIEWWIDGET_H
#define MODULEVIEWWIDGET_H
#include <yarp/manager/manager.h>
#include <QWidget>
#include "genericviewwidget.h"

namespace Ui {
class ModuleViewWidget;
}

/*! \class ModuleViewWidget
    \brief The Module View Widget
*/
class ModuleViewWidget : public GenericViewWidget
{
    Q_OBJECT

public:
    explicit ModuleViewWidget(yarp::manager::Module *mod, QWidget *parent = 0);
    ~ModuleViewWidget();

private:
    yarp::manager::Module *module;
    Ui::ModuleViewWidget *ui;
};

#endif // MODULEVIEWWIDGET_H
