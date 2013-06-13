/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "PlotManager.h"
#include "PortReader.h"
#include "Debug.h"
#include "DataPlot.h"

#include <glibmm/thread.h>
#include <gdkmm/color.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>

#include <gtkdataboxmm/lines.h>
#include <gtkdataboxmm/points.h>
#include <gtkdataboxmm/bars.h>
#include "Graph.h"


namespace {
static YarpScope::PlotManager *s_instance = NULL;
static Glib::StaticMutex s_mutex;
}

class YarpScope::PlotManager::Private
{
public:
    Private(PlotManager *p) :
        parent(p),
        table(1, 1, true)
    {
        table.set_col_spacings(4);
        table.set_row_spacings(4);
    }

    ~Private() {
        for (std::vector<DataPlot*>::iterator it = plots.begin(); it != plots.end(); it++) {
            delete *it;
        }
        for (std::vector<Graph*>::iterator it = graphs.begin(); it != graphs.end(); it++) {
            delete *it;
        }
    }
    PlotManager * const parent;

    std::vector<DataPlot*> plots;
    std::vector<Graph*> graphs;
    Gtk::Table table;
};


YarpScope::PlotManager::PlotManager() :
    mPriv(new Private(this))
{
}

YarpScope::PlotManager::~PlotManager()
{
    delete mPriv;
}

YarpScope::PlotManager& YarpScope::PlotManager::instance()
{
    // I don't know if a static Glib::Mutex is thread safe but it shouldn't be
    // used on concurrent threads during the creation, so it shouldn't be a
    // problem.
    Glib::Mutex::Lock lock(s_mutex);
    if (!s_instance) {
        s_instance = new PlotManager();
    }

    return *s_instance;
}

void YarpScope::PlotManager::setupTable(int rows, int columns)
{
    mPriv->table.resize(rows, columns);
}

int YarpScope::PlotManager::addPlot(const Glib::ustring &title,
                                    int gridx,
                                    int gridy,
                                    int hspan,
                                    int vspan,
                                    float minval,
                                    float maxval,
                                    int size,
                                    const Glib::ustring &bgcolor,
                                    bool autorescale)
{
    debug() << "Adding plot" << title << "in position" << gridx << gridy << hspan << vspan;

    Gtk::Table *table;
    YarpScope::DataPlot *newplot = new DataPlot(&table, minval, maxval, size, bgcolor, autorescale);

    Gtk::Label *label = new Gtk::Label(title);
    table->attach(*label, 0, 3, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
    mPriv->table.attach(*table, gridx, gridx + hspan, gridy, gridy + vspan);
    mPriv->table.show_all_children();

    // Lock to avoid returning the wrong index if pushing from different threads
    Glib::Mutex::Lock lock(s_mutex);
    mPriv->plots.push_back(newplot);
    return (mPriv->plots.size() - 1);
}

int YarpScope::PlotManager::addGraph(int plotIndex,
                                     const Glib::ustring &remote,
                                     int index,
                                     const Glib::ustring &title,
                                     const Glib::ustring &color,
                                     const Glib::ustring &type,
                                     int size)
{
    DataPlot *plot = NULL;
    try {
        plot = mPriv->plots.at(plotIndex);
    } catch (std::out_of_range) {
        fatal() << "Trying to add a graph to an invalid plot";
    }
    if (!plot) {
        fatal() << "Trying to add a graph to an invalid plot";
    }

    Graph *newgraph = new Graph(remote, index, title, color, type, size, plot->size());

    plot->graph_add(newgraph->graph());

    // Lock to avoid returning the wrong index if pushing from different threads
    Glib::Mutex::Lock lock(s_mutex);
    mPriv->graphs.push_back(newgraph);
    return (mPriv->plots.size() - 1); // FIXME useless
}

Gtk::Widget* YarpScope::PlotManager::getPlotWidget() const
{
    return &(mPriv->table);
}

void YarpScope::PlotManager::redraw(bool increment) const
{
    for (std::vector<Graph*>::iterator it = mPriv->graphs.begin();
                it != mPriv->graphs.end(); it++) {
        (*it)->update(increment);
    }
    for (std::vector<DataPlot*>::iterator it = mPriv->plots.begin();
                it != mPriv->plots.end(); it++) {
        //(*it)->auto_rescale(0.05);
        (*it)->queue_draw();
    }
}

void YarpScope::PlotManager::autoRescale() const
{
    for (std::vector<DataPlot*>::iterator it = mPriv->plots.begin();
                it != mPriv->plots.end(); it++) {
        (*it)->auto_rescale(0.05F);
        (*it)->queue_draw();
    }
}
