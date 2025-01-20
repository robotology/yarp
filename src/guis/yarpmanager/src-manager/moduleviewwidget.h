/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
