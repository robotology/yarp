/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARPSCOPE_MAINWINDOW_H
#define YARPSCOPE_MAINWINDOW_H

#include <gtkmm/window.h>

namespace yarp {
namespace os {
class Value;
} // namespace os
} // namespace yarp

namespace Gtk {
class ActionGroup;
class UIManager;
} // namespace Gtk

namespace YarpScope {

class MainWindow : public Gtk::Window
{
public:
    MainWindow();
    virtual ~MainWindow();

    void setInterval(int interval);

private:
    class Private;
    Private * const mPriv;
};

} // namespace YarpScope

#endif // YARPSCOPE_MAINWINDOW_H
