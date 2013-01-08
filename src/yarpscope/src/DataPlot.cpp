/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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


class YarpScope::DataPlot::Private
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

    int gridx;
    int gridy;
    int hspan;
    int vspan;
    float minval;
    float maxval;
    int size;
    Gdk::Color bgcolor;
    Gdk::Color gridcolor;
    Gdk::Color crosscolor;
    bool autorescale;

    float *hlines;
    float *vlines;
    Glib::RefPtr<GDatabox::CrossSimple> cross;
    Glib::RefPtr<GDatabox::Grid> grid;

    bool acquire;
};

void YarpScope::DataPlot::Private::createGrid()
{
    for (int i = 0; i < 9; i++) {
        hlines[i] = ((maxval + minval) / 2) + ((maxval - minval) / 8) * (i - 4);
    }
    for (int i = 0; i < 8; i++) {
        vlines[i] = size / 8.0f * (i + 1);
    }

    grid = GDatabox::Grid::create(9, 8, hlines, vlines, gridcolor);
    cross = GDatabox::CrossSimple::create(crosscolor);

    parent->graph_add(grid);
    parent->graph_add(cross);
}


YarpScope::DataPlot::DataPlot(Gtk::Table **table,
                              float minval,
                              float maxval,
                              int size,
                              const Glib::ustring &bgcolor,
                              //const Glib::ustring &gridcolor,
                              //const Glib::ustring &crosscolor,
                              bool autorescale) :
    GDatabox::Databox(table, true, true, false, true),
    mPriv(new Private(this))
{
    mPriv->minval = minval;
    mPriv->maxval = maxval;
    mPriv->size = size;
    mPriv->bgcolor = (bgcolor.empty() ? Gdk::Color("LightSlateGray") : Gdk::Color(bgcolor));
    mPriv->gridcolor = Gdk::Color("Grey");
    mPriv->crosscolor = Gdk::Color("DarkSlateGrey");
    mPriv->autorescale = autorescale;

    mPriv->createGrid();
    set_total_limits(0, (float)size, maxval, minval);
    if (autorescale) {
        auto_rescale(0.05f);
    }

    modify_bg(Gtk::STATE_NORMAL, mPriv->bgcolor);
}


YarpScope::DataPlot::~DataPlot()
{
    delete mPriv;
}

void YarpScope::DataPlot::toggleAcquire(bool toggled)
{
    mPriv->acquire = toggled;
}

int YarpScope::DataPlot::size() const
{
    return mPriv->size;
}
