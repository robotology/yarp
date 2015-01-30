/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "simpleloader.h"
#include <yarp/os/Value.h>

SimpleLoader::SimpleLoader(/* FIXME const */ yarp::os::ResourceFinder *options, PlotManager *plotManager, bool *ok, QObject *parent) :
    GenericLoader(parent)
{
    this->plotManager = plotManager;

    if (!options->check("remote")) {
           qDebug("Missing \"remote\" argument. Will wait for external connection");
    } else {
       graph_remote = QString("%1").arg(options->find("remote").toString().c_str());
    }

    local_port = default_local_port;

    if (options->check("carrier")) {
        portscope_carrier = QString("%1").arg(options->find("carrier").asString().c_str());
    } else {
        portscope_carrier = default_portscope_carrier;
    }

    if (options->check("no-persistent")) {
        portscope_persistent = false;
    } else if (options->check("persistent")) {
        portscope_persistent = true;
    } else {
        portscope_persistent = default_portscope_persistent;
    }


    if (!options->check("index")) {
        qWarning() << "Missing \"index\" argument. Will use index = 0";
    }
    const yarp::os::Value &indexValue = options->find("index");


    if (options->check("plot_title")) {
        plot_title = QString("%1").arg(options->find("plot_title").toString().c_str());
    } else {
        plot_title = graph_remote;
    }

    if (options->check("min")) {
        plot_minval = (float)options->find("min").asDouble();
    } else {
        plot_minval = default_plot_minval;
    }

    if (options->check("max")) {
        plot_maxval = (float)options->find("max").asDouble();
    } else {
        plot_maxval = default_plot_maxval;
    }

    if (options->check("size")) {
        plot_size = options->find("size").asInt();
    } else {
        plot_size = default_plot_size;
    }

    if (options->check("bgcolor")) {
        plot_bgcolor = QString("%1").arg(options->find("bgcolor").asString().c_str());
    } else {
        plot_bgcolor = default_bg_color;
    }

    plot_autorescale = options->check("autorescale");

    // TODO enable realtime mode
    plot_realtime = options->check("realtime");
    Q_UNUSED(plot_realtime); // UNUSED

    // TODO enable trigger mode
    plot_triggermode = options->check("triggermode");
    Q_UNUSED(plot_triggermode); // UNUSED

    Plotter *plotter = plotManager->addPlot(plot_title, 0, 0, 1, 1, plot_minval, plot_maxval, plot_size, plot_bgcolor, plot_autorescale);


    graph_color = default_graph_color;

    if (!indexValue.isList()) {
        // SINGLE PLOT
        graph_index = indexValue.asInt();

        if (options->check("graph_title")) {
            if (options->find("graph_title").isList()) {
                qCritical("\"graph_title\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            graph_title = QString("%1").arg(options->find("graph_title").toString().c_str());
        }

        if (options->check("color")) {
            if (options->find("color").isList()) {
                qCritical("\"color\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            graph_color = QString("%1").arg(options->find("color").toString().c_str());
        }


        if (options->check("type")) {
            if (options->find("type").isList()) {
                qCritical("\"type\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            graph_type =  QString("%1").arg(options->find("type").toString().c_str());
        } else {
            graph_type = default_graph_type;
        }

        if (options->check("graph_size")) {
            if (options->find("graph_size").isList()) {
                qCritical("\"graph_size\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            graph_size =  options->find("graph_size").asInt();
        } else {
            graph_size = default_graph_size;
        }
        plotter->init(graph_remote, local_port, portscope_carrier, portscope_persistent);
        plotter->addGraph(graph_index,graph_title, graph_color, graph_type, graph_size);

    } else {
        const yarp::os::Bottle &indexes = *indexValue.asList();
        yarp::os::Bottle titles, colors, types, sizes;

        if (options->check("graph_title")) {
            const yarp::os::Value &titlesValue = options->find("graph_title");
            if (!titlesValue.isList()) {
                qCritical("\"graph_title\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            titles = *titlesValue.asList();
            if (titles.size() != indexes.size()) {
                qCritical("\"graph_title\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
        } else {
            titles = yarp::os::Bottle::getNullBottle();
        }

        if (options->check("color")) {
            const yarp::os::Value &colorsValue = options->find("color");
            if (!colorsValue.isList()) {
                qCritical("\"color\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            colors = *colorsValue.asList();
            if (colors.size() != indexes.size()) {
                qCritical("\"color\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
        } else {
            colors = yarp::os::Bottle::getNullBottle();
        }

        if (options->check("type")) {
            const yarp::os::Value &typesValue = options->find("type");
            if (!typesValue.isList()) {
                qCritical("\"type\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            types = *typesValue.asList();
            if (types.size() != indexes.size()) {
                qCritical("\"type\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
        } else {
            types = yarp::os::Bottle::getNullBottle();
        }

        if (options->check("graph_size")) {
            const yarp::os::Value &sizesValue = options->find("graph_size");
            if (!sizesValue.isList()) {
                qCritical("\"graph_size\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
            sizes = *sizesValue.asList();
            if (sizes.size() != indexes.size()) {
                qCritical("\"graph_size\" and \"index\" arguments should have the same number of elements");
                *ok = false;
                return;
            }
        } else {
            sizes = yarp::os::Bottle::getNullBottle();
        }

        plotter->init(graph_remote,  local_port, portscope_carrier, portscope_persistent);

        for (int i = 0; i < indexes.size(); i++) {
            graph_index = indexes.get(i).asInt();

            if (!titles.isNull()) {
                graph_title = QString("%1").arg(titles.get(i).asString().data());
            }

            if (!colors.isNull()) {
                graph_color = QString("%1").arg(colors.get(i).asString().data());
            }

            if (!types.isNull()) {
                graph_type = QString("%1").arg(types.get(i).asString().data());
            } else {
                graph_type = default_graph_type;
            }

            if (!sizes.isNull()) {
                graph_size = sizes.get(i).asInt();
            } else {
                graph_size = default_graph_size;
            }
            plotter->addGraph( graph_index, graph_title, graph_color, graph_type, graph_size);
        }
    }
    *ok = true;

}
