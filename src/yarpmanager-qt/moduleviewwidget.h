/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef MODULEVIEWWIDGET_H
#define MODULEVIEWWIDGET_H
#include "manager.h"
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
    explicit ModuleViewWidget(Module *mod, QWidget *parent = 0);
    ~ModuleViewWidget();

private:
    Module *module;
    Ui::ModuleViewWidget *ui;
};

#endif // MODULEVIEWWIDGET_H
