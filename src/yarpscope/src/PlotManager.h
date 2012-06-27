/*
 *  This file is part of gPortScope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef GPORTSCOPE_PLOTMANAGER_H
#define GPORTSCOPE_PLOTMANAGER_H


namespace Glib {
class ustring;
}

namespace Gtk {
class Widget;
}

namespace GPortScope
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
    void redraw() const;

private:

    // not implemented
    PlotManager& operator=(const PlotManager &other);
    PlotManager(const PlotManager &other);

    class Private;
    Private * const mPriv;
};

} // namespace GPortScope


#endif // GPORTSCOPE_PLOTMANAGER_H
