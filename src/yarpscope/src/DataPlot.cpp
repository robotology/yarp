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

#include "DataPlot.h"
#include "Debug.h"

#include <gtkdataboxmm/lines.h>
#include <gtkdataboxmm/bars.h>
#include <gtkdataboxmm/points.h>
#include <gtkdataboxmm/cross_simple.h>
#include <gtkdataboxmm/grid.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>


class GPortScope::DataPlot::Private
{
public:
    Private(DataPlot *parent):
        parent(parent),
        hlines(new float[9]),
        vlines(new float[8])
    {
    }

    ~Private()
    {
        delete [] hlines;
        delete [] vlines;
   }

    void createGrid();

    // parent DataPlot
    DataPlot * const parent;

    Glib::ustring title;
    int gridx;
    int gridy;
    int hspan;
    int vspan;
    int minval;
    int maxval;
    int size;
    Gdk::Color bgcolor;
    bool autorescale;

    float *hlines;
    float *vlines;
    Glib::RefPtr<GDatabox::CrossSimple> cross;
    Glib::RefPtr<GDatabox::Grid> grid;

    bool acquire;
};

void GPortScope::DataPlot::Private::createGrid()
{
    for (int i = 0; i < 9; i++) {
        hlines[i] = ((maxval + minval) / 2) + ((maxval - minval) / 8) * (i - 4);
    }
    for (int i = 0; i < 8; i++) {
        vlines[i] = size / 8.0 * (i + 1);
    }

    grid = GDatabox::Grid::create(9, 8, hlines, vlines, Gdk::Color("Grey"));
    cross = GDatabox::CrossSimple::create(Gdk::Color("DarkSlateGrey"));

    parent->graph_add(grid);
    parent->graph_add(cross);
}


GPortScope::DataPlot::DataPlot(const Glib::ustring &title, //FIXME NEEDED?
                               int minval,
                               int maxval,
                               int size,
                               const Glib::ustring &bgcolor,
                               bool autorescale) :
    mPriv(new Private(this))
{
    mPriv->title = title;
    mPriv->minval = minval;
    mPriv->maxval = maxval;
    mPriv->size = size;
    mPriv->bgcolor = (bgcolor.empty() ? Gdk::Color("LightSlateGray") : Gdk::Color(bgcolor));
    mPriv->autorescale = autorescale;

    mPriv->createGrid();
    set_total_limits(-5., size + 5., maxval + 5, minval - 5);
    if (autorescale) {
        auto_rescale(0.05);
    }

    modify_bg(Gtk::STATE_NORMAL, mPriv->bgcolor);
}


GPortScope::DataPlot::~DataPlot()
{
    delete mPriv;
}

void GPortScope::DataPlot::toggleAcquire(bool toggled)
{
    mPriv->acquire = toggled;
}

int GPortScope::DataPlot::size() const
{
    return mPriv->size;
}
