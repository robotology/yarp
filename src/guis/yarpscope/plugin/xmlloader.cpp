/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "xmlloader.h"
#include <QXmlStreamReader>
#include <QFile>

XmlLoader::XmlLoader(QString fileName, PlotManager *plotManager,QObject *parent) : GenericLoader(parent)
{
    plotter = nullptr;
    this->plotManager = plotManager;

    auto* file = new QFile(fileName);
       /* If we can't open it, let's show an error message. */
       if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
           return;
       }

    QXmlStreamReader xml(file);

    while(!xml.atEnd() && !xml.hasError()) {


        QXmlStreamReader::TokenType token = xml.readNext();

        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if(token == QXmlStreamReader::StartElement) {
            if(xml.name() == "portscope") {
                QXmlStreamAttributes attributes = xml.attributes();

                if(attributes.hasAttribute("rows")) {
                    portscope_rows = attributes.value("rows").toInt();
                }
                if(attributes.hasAttribute("columns")) {
                    portscope_columns = attributes.value("columns").toInt();
                }
                if(attributes.hasAttribute("carrier")) {
                    portscope_carrier = attributes.value("carrier").toString();
                }
                if(attributes.hasAttribute("persistent")) {
                     portscope_persistent = QVariant(attributes.value("persistent").toString()).toBool();
                }

                continue;
            }

            if (xml.name() == "plot") {

                QXmlStreamAttributes plotAttributes = xml.attributes();
                int plot_gridx(default_plot_gridx);
                int plot_gridy(default_plot_gridy);
                int plot_hspan(default_plot_hspan);
                int plot_vspan(default_plot_vspan);
                int plot_size(default_plot_size);
                float plot_minval(default_plot_minval);
                float plot_maxval(default_plot_maxval);
                bool plot_autorescale(default_plot_autorescale);
                bool plot_realtime(default_plot_realtime);
                bool plot_triggermode(default_plot_triggermode);
                QString plot_bgcolor(default_plot_bgcolor);
                QString plot_title(default_plot_title);

                if(plotAttributes.hasAttribute("title")) {
                    plot_title = plotAttributes.value("title").toString();
                }
                if(plotAttributes.hasAttribute("gridx")) {
                    plot_gridx = plotAttributes.value("gridx").toInt();
                }
                if(plotAttributes.hasAttribute("gridy")) {
                    plot_gridy = plotAttributes.value("gridy").toInt();
                }
                if(plotAttributes.hasAttribute("hspan")) {
                    plot_hspan = plotAttributes.value("hspan").toInt();
                }
                if(plotAttributes.hasAttribute("vspan")) {
                    plot_vspan = plotAttributes.value("vspan").toInt();
                }
                if(plotAttributes.hasAttribute("minval")) {
                    plot_minval = plotAttributes.value("minval").toFloat();
                }
                if(plotAttributes.hasAttribute("maxval")) {
                    plot_maxval = plotAttributes.value("maxval").toFloat();
                }
                if(plotAttributes.hasAttribute("size")) {
                    plot_size = plotAttributes.value("size").toInt();
                }
                if(plotAttributes.hasAttribute("bgcolor")) {
                    plot_bgcolor = plotAttributes.value("bgcolor").toString();
                }
                if(plotAttributes.hasAttribute("autorescale")) {
                    plot_autorescale = QVariant(plotAttributes.value("autorescale").toString()).toBool();
                }
                if(plotAttributes.hasAttribute("realtime")) {
                    plot_realtime = QVariant(plotAttributes.value("realtime").toString()).toBool();
                }
                if(plotAttributes.hasAttribute("triggermode")) {
                    plot_triggermode = QVariant(plotAttributes.value("triggermode").toString()).toBool();
                }

                plotter = plotManager->addPlot(plot_title, plot_gridx, plot_gridy, plot_hspan, plot_vspan, plot_minval, plot_maxval, plot_size, plot_bgcolor, plot_autorescale);
                Q_UNUSED(plot_realtime);
                Q_UNUSED(plot_triggermode);
                continue;
            }

            if (xml.name() == "graph") {

                QXmlStreamAttributes graphAttributes = xml.attributes();
                QString graph_remote;
                QString graph_title(default_graph_title);
                QString graph_color(default_graph_color);
                QString graph_type(default_graph_type);
                int graph_index;
                double graph_y_scale;
                int graph_size(default_graph_size);

                if(graphAttributes.hasAttribute("remote")) {
                    graph_remote = graphAttributes.value("remote").toString();
                } else {
                    qCritical(R"(Syntax error while loading %s. "remote" attribute not found in element "graph")",fileName.toLatin1().data());
                    break;
                }
                if(graphAttributes.hasAttribute("index")) {
                    graph_index = graphAttributes.value("index").toInt();
                    if(graph_index < 0) {
                        qCritical(R"(Syntax error while loading %s. "index" attribute not found in element "graph")",fileName.toLatin1().data());
                        break;
                    }
                } else {
                    qCritical(R"(Syntax error while loading %s. "index" attribute in element "graph" should be >= 0)",fileName.toLatin1().data());
                    break;
                }
                if(graphAttributes.hasAttribute("title")) {
                    graph_title = graphAttributes.value("title").toString();
                }
                if(graphAttributes.hasAttribute("rescale_y_factor")) {
                    graph_y_scale = graphAttributes.value("rescale_y_factor").toDouble();
                }
                else
                {
                    graph_y_scale = 1.0;
                }
                if(graphAttributes.hasAttribute("color")) {
                    graph_color = graphAttributes.value("color").toString();
                }
                if(graphAttributes.hasAttribute("type")) {
                    graph_type = graphAttributes.value("type").toString();
                }
                if(graphAttributes.hasAttribute("size")) {
                    graph_size = graphAttributes.value("size").toInt();
                    if(graph_size <= 0){
                        graph_size = default_graph_size;
                    }
                }
                if (plotter)
                {
                    Graph *graph = plotter->addGraph(graph_remote,"",graph_index,graph_title, graph_color, graph_type, graph_size, graph_y_scale);
                    if(graph){
                        graph->init(graph_remote,"", portscope_carrier, portscope_persistent);
                    }
                }
                continue;
            }

        }
    }
}
