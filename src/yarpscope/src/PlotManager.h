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


#ifndef YARPSCOPE_PLOTMANAGER_H
#define YARPSCOPE_PLOTMANAGER_H


namespace Glib {
class ustring;
}

namespace Gtk {
class Widget;
}

namespace YarpScope
{

class PlotManager
{
protected:
    PlotManager();

public:
    virtual ~PlotManager();

    static PlotManager& instance();

    int addPlot(const Glib::ustring &title,
                int gridx,
                int gridy,
                int hspan,
                int vspan,
                int minval,
                int maxval,
                int size,
                const Glib::ustring &bgcolor,
                bool autorescale);

    int addGraph(int portIndex,
                 const Glib::ustring &remote,
                 int index,
                 const Glib::ustring &title,
                 const Glib::ustring &color,
                 const Glib::ustring &type,
                 int size);

    Gtk::Widget *getPlotWidget() const;
    void redraw(bool increment = true) const;

private:

    // not implemented
    PlotManager& operator=(const PlotManager &other);
    PlotManager(const PlotManager &other);

    class Private;
    Private * const mPriv;
};

} // namespace YarpScope


#endif // YARPSCOPE_PLOTMANAGER_H
