/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "xmlloader.h"
#include <QXmlStreamReader>
#include <QFile>

XmlLoader::XmlLoader(QString fileName, PlotManager *plotManager,QObject *parent) : GenericLoader(parent)
{
    this->plotManager = plotManager;

    QFile* file = new QFile(fileName);
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

            if (xml.name() == "plot"){

                QXmlStreamAttributes plotAttributes = xml.attributes();
                int plot_gridx, plot_gridy, plot_hspan, plot_vspan, plot_size;
                float plot_minval, plot_maxval;
                bool plot_autorescale, plot_realtime, plot_triggermode;

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
                    plot_minval = plotAttributes.value("minval").toInt();
                }
                if(plotAttributes.hasAttribute("maxval")) {
                    plot_maxval = plotAttributes.value("maxval").toInt();
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

            if (xml.name() == "graph"){
                QXmlStreamAttributes graphAttributes = xml.attributes();
                if(graphAttributes.hasAttribute("remote")) {
                    graph_remote = graphAttributes.value("remote").toString();
                }else{
                    qCritical("Syntax error while loading %s. \"remote\" attribute not found in element \"graph\"",fileName.toLatin1().data());
                    break;
                }
                if(graphAttributes.hasAttribute("index")) {
                    graph_index = graphAttributes.value("index").toInt();
                    if(graph_index < 0){
                        qCritical("Syntax error while loading %s. \"index\" attribute not found in element \"graph\"",fileName.toLatin1().data());
                        break;
                    }
                }else{
                    qCritical("Syntax error while loading %s. \"index\" attribute in element \"graph\" should be >= 0",fileName.toLatin1().data());
                    break;
                }
                if(graphAttributes.hasAttribute("title")) {
                    graph_title = graphAttributes.value("title").toString();
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
                plotter->init(graph_remote,"", portscope_carrier, portscope_persistent);
                plotter->addGraph(graph_index,graph_title, graph_color, graph_type, graph_size);
                continue;
            }

        }
    }
}
