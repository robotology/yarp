/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "SimpleLoader.h"
#include "PortReader.h"
#include "PlotManager.h"
#include "Debug.h"

#include <yarp/os/Property.h>
#include <yarp/os/Value.h>
#include <string.h>

namespace {
    static const Glib::ustring default_local_port = "/yarpscope"; // FIXME Use "..."
    static const float default_plot_minval = -100.;
    static const float default_plot_maxval = 100.;
    static const int default_plot_size = 201;
    static const int default_graph_size = 1;
    static const Glib::ustring default_graph_type = "lines";
    static const Glib::ustring default_connection_carrier = "mcast";
    static bool default_connection_persistent = true;
}


YarpScope::SimpleLoader::SimpleLoader(/* FIXME const */ yarp::os::Property &options, bool *ok)
{
    YarpScope::PortReader &portReader = YarpScope::PortReader::instance();
    YarpScope::PlotManager &plotManager = YarpScope::PlotManager::instance();

    Glib::ustring graph_remote, local_port, connection_carrier;
    bool connection_persistent;

    if (!options.check("remote")) {
        debug() << "Missing \"remote\" argument. Will wait for external connection";
    } else {
        graph_remote = options.find("remote").toString().c_str();
    }

    local_port = default_local_port;

    if (options.check("carrier")) {
        connection_carrier = options.find("carrier").asString().c_str();
    } else {
        connection_carrier = default_connection_carrier;
    }

    // TODO read from command line whether connections should be persistent or not
    connection_persistent = default_connection_persistent;


    if (!options.check("index")) {
        warning() << "Missing \"index\" argument. Will use index = 0";
    }
    const yarp::os::Value &indexValue = options.find("index");

    Glib::ustring plot_title, plot_bgcolor;
    int plot_size;
    float plot_minval, plot_maxval;
    bool plot_autorescale, plot_realtime, plot_triggermode;

    if (options.check("plot_title")) {
        plot_title = options.find("plot_title").toString().c_str();
    } else {
        plot_title = graph_remote;
    }

    if (options.check("min")) {
        plot_minval = (float)options.find("min").asDouble();
    } else {
        plot_minval = default_plot_minval;
    }

    if (options.check("max")) {
        plot_maxval = (float)options.find("max").asDouble();
    } else {
        plot_maxval = default_plot_maxval;
    }

    if (options.check("size")) {
        plot_size = options.find("size").asInt();
    } else {
        plot_size = default_plot_size;
    }

    if (options.check("bgcolor")) {
        plot_bgcolor = options.find("bgcolor").asString().c_str();
    }

    plot_autorescale = options.check("autorescale");

    // TODO enable realtime mode
    plot_realtime = options.check("realtime");
    (void)plot_realtime; // UNUSED

    // TODO enable trigger mode
    plot_triggermode = options.check("triggermode");
    (void)plot_triggermode; // UNUSED

    int plotIndex = plotManager.addPlot(plot_title, 0, 0, 1, 1, plot_minval, plot_maxval, plot_size, plot_bgcolor, plot_autorescale);


    Glib::ustring graph_title, graph_color, graph_type;
    int graph_index, graph_size;

    if (!indexValue.isList()) {

        graph_index = indexValue.asInt();

        if (options.check("graph_title")) {
            if (options.find("graph_title").isList()) {
                error() << "\"graph_title\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            graph_title = options.find("graph_title").toString().c_str();
        }

        if (options.check("color")) {
            if (options.find("color").isList()) {
                error() << "\"color\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            graph_color = options.find("color").toString().c_str();
        }

        if (options.check("type")) {
            if (options.find("type").isList()) {
                error() << "\"type\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            graph_type =  options.find("type").toString().c_str();
        } else {
            graph_type = default_graph_type;
        }

        if (options.check("graph_size")) {
            if (options.find("graph_size").isList()) {
                error() << "\"graph_size\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            graph_size =  options.find("graph_size").asInt();
        } else {
            graph_size = default_graph_size;
        }

        portReader.acquireData(graph_remote, graph_index, local_port, connection_carrier, connection_persistent);
        plotManager.addGraph(plotIndex, graph_remote, graph_index, graph_title, graph_color, graph_type, graph_size);

    } else {
        const yarp::os::Bottle &indexes = *indexValue.asList();
        yarp::os::Bottle titles, colors, types, sizes;

        if (options.check("graph_title")) {
            const yarp::os::Value &titlesValue = options.find("graph_title");
            if (!titlesValue.isList()) {
                error() << "\"graph_title\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            titles = *titlesValue.asList();
            if (titles.size() != indexes.size()) {
                error() << "\"graph_title\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
        } else {
            titles = yarp::os::Bottle::getNullBottle();
        }

        if (options.check("color")) {
            const yarp::os::Value &colorsValue = options.find("color");
            if (!colorsValue.isList()) {
                error() << "\"color\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            colors = *colorsValue.asList();
            if (colors.size() != indexes.size()) {
                error() << "\"color\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
        } else {
            colors = yarp::os::Bottle::getNullBottle();
        }

        if (options.check("type")) {
            const yarp::os::Value &typesValue = options.find("type");
            if (!typesValue.isList()) {
                error() << "\"type\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            types = *typesValue.asList();
            if (types.size() != indexes.size()) {
                error() << "\"type\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
        } else {
            types = yarp::os::Bottle::getNullBottle();
        }

        if (options.check("graph_size")) {
            const yarp::os::Value &sizesValue = options.find("graph_size");
            if (!sizesValue.isList()) {
                error() << "\"graph_size\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
            sizes = *sizesValue.asList();
            if (sizes.size() != indexes.size()) {
                error() << "\"graph_size\" and \"index\" arguments should have the same number of elements";
                *ok = false;
                return;
            }
        } else {
            sizes = yarp::os::Bottle::getNullBottle();
        }


        for (int i = 0; i < indexes.size(); i++) {
            graph_index = indexes.get(i).asInt();

            if (!titles.isNull()) {
                graph_title = titles.get(i).asString();
            }

            if (!colors.isNull()) {
                graph_color = colors.get(i).asString();
            }

            if (!types.isNull()) {
                graph_type = types.get(i).asString();
            } else {
                graph_type = default_graph_type;
            }

            if (!sizes.isNull()) {
                graph_size = sizes.get(i).asInt();
            } else {
                graph_size = default_graph_size;
            }

            portReader.acquireData(graph_remote, graph_index, local_port, connection_carrier, connection_persistent);
            plotManager.addGraph(plotIndex, graph_remote, graph_index, graph_title, graph_color, graph_type, graph_size);
        }
    }
    *ok = true;
}

YarpScope::SimpleLoader::~SimpleLoader()
{
}
