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

#include <gtkdataboxmm/lines.h>
#include <gtkdataboxmm/bars.h>
#include <gtkdataboxmm/points.h>
#include <gtkdataboxmm/cross_simple.h>

#define POINTS 2000

namespace GPortScope {

class DataPlot::Private
{
public:
    Private(DataPlot *parent):
        parent(parent),
        X(new float[POINTS]),
        Y_lines(new float[POINTS]),
        Y_bars(new float[POINTS]),
        Y_points(new float[POINTS])

    {
    }

    ~Private()
    {
        delete X;
        delete Y_lines;
        delete Y_bars;
        delete Y_points;
    }

    // parent DataPlot
    DataPlot * const parent;

    // Graphs
    Glib::RefPtr<GtkDataboxMM::Bars> bars;
    Glib::RefPtr<GtkDataboxMM::Graph> lines;
    Glib::RefPtr<GtkDataboxMM::Points> points;
    Glib::RefPtr<GtkDataboxMM::CrossSimple> cross;

    // Points
    float *X;
    float *Y_lines;
    float *Y_bars;
    float *Y_points;


};

DataPlot::DataPlot() :
    mPriv(new Private(this))
{
//    set_enable_selection(false);
//    set_enable_zoom(false);
    modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#EB00EB00EB00"));

    for (int i = 0; i < POINTS; i++) {
        mPriv->X[i] = i;
        mPriv->Y_lines[i] = cos (i * 4 * G_PI / POINTS) / 2;
        mPriv->Y_bars[i] = sin(i * 4 * G_PI / POINTS);
        mPriv->Y_points[i] = ((float)rand() / RAND_MAX) * 2 - 1;
    }

    mPriv->lines = GtkDataboxMM::Lines::create(POINTS, mPriv->X, mPriv->Y_lines, Gdk::Color("Red"), 3);
    mPriv->bars = GtkDataboxMM::Bars::create(POINTS, mPriv->X, mPriv->Y_bars, Gdk::Color("Green"));
    mPriv->points = GtkDataboxMM::Points::create(POINTS, mPriv->X, mPriv->Y_points, Gdk::Color("Blue"), 5);
    mPriv->cross = GtkDataboxMM::CrossSimple::create(Gdk::Color("Black"));

    graph_add(mPriv->cross);
    graph_add(mPriv->lines);
    graph_add(mPriv->bars);
    graph_add(mPriv->points);

   /* Create the GtkDatabox widget */
//   gtk_databox_create_box_with_scrollbars_and_rulers (&box, &table,
//                               TRUE, TRUE, FALSE, FALSE);

    set_total_limits(-1000., 5000., -10000., 23000.);
    auto_rescale(0.05);
}

DataPlot::~DataPlot()
{
    delete mPriv;
}


} // namespace GPortScope

#include <gtkdatabox.h>
