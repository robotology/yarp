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
/*
    QString plot_bgcolor;
    int plot_size;
    float plot_minval;
    float plot_maxval;
    bool plot_autorescale;
    bool plot_realtime;
    bool plot_triggermode;
    QString plot_title;

    QString graph_localport;
    QString graph_remote;
    QString graph_title;
    QString graph_color;
    QString graph_type;
    int graph_index;
    int graph_size;*/

SimpleLoader::SimpleLoader(/* FIXME const */ yarp::os::ResourceFinder *options, PlotManager *plotManager, bool *ok, QObject *parent) :
    GenericLoader(parent),
    plotManager(plotManager),
    plot_bgcolor(default_plot_bgcolor),
    plot_size(default_plot_size),
    plot_minval(default_plot_minval),
    plot_maxval(default_plot_maxval),
    plot_autorescale(options->check("autorescale")),
    plot_realtime(options->check("realtime")),
    plot_triggermode(options->check("triggermode")),
    plot_title(default_plot_title),
    graph_remote(),
    graph_index(-1),
    graph_localport(default_portscope_localport),
    graph_title(default_graph_title),
    graph_color(default_graph_color),
    graph_type(default_graph_type),
    graph_size(default_graph_size)
{
    if (!options->check("remote")) {
           qDebug("Missing \"remote\" argument. Will wait for external connection");
    } else {
       graph_remote = QString("%1").arg(options->find("remote").toString().c_str());
       plot_title = graph_remote;
    }

    if (options->check("carrier")) {
        portscope_carrier = QString("%1").arg(options->find("carrier").asString().c_str());
    }
    if (options->check("no-persistent")) {
        portscope_persistent = false;
    } else if (options->check("persistent")) {
        portscope_persistent = true;
    }

    if (!options->check("index")) {
        qWarning() << "Missing \"index\" argument. Will use index = 0";
    }
    const yarp::os::Value &indexValue = options->find("index");


    if (options->check("plot_title")) {
        plot_title = QString("%1").arg(options->find("plot_title").toString().c_str());
    }

    if (options->check("min")) {
        plot_minval = (float)options->find("min").asDouble();
    }

    if (options->check("max")) {
        plot_maxval = (float)options->find("max").asDouble();
    }

    if (options->check("size")) {
        plot_size = options->find("size").asInt();
    }

    if (options->check("bgcolor")) {
        plot_bgcolor = QString("%1").arg(options->find("bgcolor").asString().c_str());
    }

    // TODO enable realtime mode
    Q_UNUSED(plot_realtime); // UNUSED

    // TODO enable trigger mode
    Q_UNUSED(plot_triggermode); // UNUSED

    Plotter *plotter = plotManager->addPlot(plot_title, 0, 0, 1, 1, plot_minval, plot_maxval, plot_size, plot_bgcolor, plot_autorescale);


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

        Graph *graph = plotter->addGraph(graph_remote, graph_localport,graph_index, graph_title, graph_color, graph_type, graph_size);
        if(graph){
            graph->init(graph_remote, graph_localport, portscope_carrier, portscope_persistent);
        }

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
            }

            if (!sizes.isNull()) {
                graph_size = sizes.get(i).asInt();
            } else {
                graph_size = default_graph_size;
            }
            Graph *graph = plotter->addGraph( graph_remote,  graph_localport,graph_index, graph_title, graph_color, graph_type, graph_size);
            if(graph){
                graph->init(graph_remote,  graph_localport, portscope_carrier, portscope_persistent);
            }
        }
    }
    *ok = true;

}
