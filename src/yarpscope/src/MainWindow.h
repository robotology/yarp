/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
