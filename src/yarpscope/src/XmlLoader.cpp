/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "XmlLoader.h"
#include "PortReader.h"
#include "PlotManager.h"
#include "Debug.h"

#include <yarp/os/ContactStyle.h>

#include <tinyxml.h>

#include <glibmm/ustring.h>
#include <stdexcept>

namespace {
    static const int default_portscope_rows = 1;
    static const int default_portscope_columns = 1;
    static const Glib::ustring default_portscope_carrier = "udp";
    static bool default_portscope_persistent = true;
    static const int default_plot_gridx = -1;
    static const int default_plot_gridy = -1;
    static const int default_plot_hspan = 1;
    static const int default_plot_vspan = 1;
    static const float default_plot_minval = -100.;
    static const float default_plot_maxval = 100.;
    static const int default_plot_size = 201;
    static const bool default_plot_autorescale = false;
    static const bool default_plot_realtime = false;
    static const bool default_plot_triggermode = false;
    static const int default_graph_size = 1;
    static const Glib::ustring default_graph_type = "lines";
}

// FIXME check if rows and columns are used, or if the table is resized
//       automatically
// FIXME set default plot bgcolor
// FIXME set default graph color


YarpScope::XmlLoader::XmlLoader(const Glib::ustring& filename)
{
    YarpScope::PortReader &portReader = YarpScope::PortReader::instance();
    YarpScope::PlotManager &plotManager = YarpScope::PlotManager::instance();


    TiXmlDocument doc(filename.c_str());
    if (!doc.LoadFile()) {
        fatal() << "Syntax error while loading" << filename << "at line" << doc.ErrorRow() << ":" << doc.ErrorDesc();
    }

    TiXmlElement *rootElem = doc.RootElement();
    if (!rootElem) {
        fatal() << "Syntax error while loading" << filename << ". No root element.";
    }

    if (Glib::ustring(rootElem->Value()).compare("portscope") != 0) {
        fatal() << "Syntax error while loading" << filename << ". Root element should be \"portscope\", found" << rootElem->Value();
    }

    int portscope_rows, portscope_columns;
    Glib::ustring portscope_carrier;
    bool portscope_persistent;

    if (rootElem->QueryIntAttribute("rows", &portscope_rows) != TIXML_SUCCESS || portscope_rows < 1) {
        portscope_rows = default_portscope_rows;
    }

    if (rootElem->QueryIntAttribute("columns", &portscope_columns) != TIXML_SUCCESS || portscope_columns < 1) {
        portscope_columns = default_portscope_columns;
    }

    if (portscope_rows != 1 || portscope_columns != 1) {
        plotManager.setupTable(portscope_rows, portscope_columns);
    }


    if (const char *t = rootElem->Attribute("carrier")) {
        portscope_carrier = t;
    } else {
        portscope_carrier = default_portscope_carrier;
    }

    if (rootElem->QueryBoolAttribute("persistent", &portscope_persistent) != TIXML_SUCCESS) {
        portscope_persistent = default_portscope_persistent;
    }

    for (TiXmlElement *plotElem = rootElem->FirstChildElement(); plotElem != 0; plotElem = plotElem->NextSiblingElement()) {
        if (Glib::ustring(plotElem->Value()).compare("plot") != 0) {
            fatal() << "Syntax error while loading" << filename << ". Expected \"plot\", found" << plotElem->Value();
        }

        Glib::ustring plot_title, plot_bgcolor;
        int plot_gridx, plot_gridy, plot_hspan, plot_vspan, plot_size;
        float plot_minval, plot_maxval;
        bool plot_autorescale, plot_realtime, plot_triggermode;
        if (const char *t = plotElem->Attribute("title")) {
            plot_title = t;
        }
        if (plotElem->QueryIntAttribute("gridx", &plot_gridx) != TIXML_SUCCESS || plot_gridx < 0) {
            plot_gridx = default_plot_gridx;
        }
        if (plotElem->QueryIntAttribute("gridy", &plot_gridy) != TIXML_SUCCESS || plot_gridy < 0) {
            plot_gridy = default_plot_gridy;
        }
        if (plotElem->QueryIntAttribute("hspan", &plot_hspan) != TIXML_SUCCESS || plot_hspan <= 0) {
            plot_hspan = default_plot_hspan;
        }
        if (plotElem->QueryIntAttribute("vspan", &plot_vspan) != TIXML_SUCCESS || plot_vspan <= 0) {
            plot_vspan = default_plot_vspan;
        }
        if (plotElem->QueryFloatAttribute("minval", &plot_minval) != TIXML_SUCCESS) {
            plot_minval = default_plot_minval;
        }
        if (plotElem->QueryFloatAttribute("maxval", &plot_maxval) != TIXML_SUCCESS) {
            plot_maxval = default_plot_maxval;
        }
        if (plotElem->QueryIntAttribute("size", &plot_size) != TIXML_SUCCESS || plot_size <= 0) {
            plot_size = default_plot_size;
        }
        if (const char *t = plotElem->Attribute("bgcolor")) {
            plot_bgcolor = t;
        }
        if (plotElem->QueryBoolAttribute("autorescale", &plot_autorescale) != TIXML_SUCCESS) {
            plot_autorescale = default_plot_autorescale;
        }
        // TODO enable realtime mode
        if (plotElem->QueryBoolAttribute("realtime", &plot_realtime) != TIXML_SUCCESS) {
            plot_realtime = default_plot_realtime;
        }
        // TODO enable trigger mode
        if (plotElem->QueryBoolAttribute("triggermode", &plot_triggermode) != TIXML_SUCCESS) {
            plot_triggermode = default_plot_triggermode;
        }


        int plotIndex = plotManager.addPlot(plot_title, plot_gridx, plot_gridy, plot_hspan, plot_vspan, plot_minval, plot_maxval, plot_size, plot_bgcolor, plot_autorescale);

        for (TiXmlElement* graphElem = plotElem->FirstChildElement(); graphElem != 0; graphElem = graphElem->NextSiblingElement()) {
            if (Glib::ustring(graphElem->Value()).compare("graph") != 0) {
                fatal() << "Syntax error while loading" << filename << ". Expected \"graph\", found" << graphElem->Value();
            }

            Glib::ustring graph_remote, graph_title, graph_color, graph_type;
            int graph_index, graph_size;
            if (const char *t = graphElem->Attribute("remote")) {
                graph_remote = t;
            } else {
                fatal() << "Syntax error while loading" << filename << ". \"remote\" attribute not found in element \"graph\"";
            }
            if (graphElem->QueryIntAttribute("index", &graph_index) != TIXML_SUCCESS) {
                fatal() << "Syntax error while loading" << filename << ". \"index\" attribute not found in element \"graph\"";
            }
            if (graph_index < 0) {
                fatal() << "Syntax error while loading" << filename << ". \"index\" attribute in element \"graph\" should be >= 0";
            }
            if (const char *t = graphElem->Attribute("title")) {
                graph_title = t;
            }
            if (const char *t = graphElem->Attribute("color")) {
                graph_color = t;
            }
            if (const char *t = graphElem->Attribute("type")) {
                graph_type = t;
            } else {
                graph_type = default_graph_type;
            }
            if (graphElem->QueryIntAttribute("size", &graph_size) != TIXML_SUCCESS || graph_size <= 0) {
                graph_size = default_graph_size;
            }

            portReader.acquireData(graph_remote, graph_index, "", portscope_carrier, portscope_persistent);
            plotManager.addGraph(plotIndex, graph_remote, graph_index, graph_title, graph_color, graph_type, graph_size);
        }
    }

    portReader.toggleAcquire(true);
}

YarpScope::XmlLoader::~XmlLoader()
{
}
