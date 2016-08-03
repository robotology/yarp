/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "genericloader.h"

GenericLoader::GenericLoader(QObject *parent) :
    QObject(parent),
    default_portscope_rows(1),
    default_portscope_columns(1),
    default_portscope_carrier("udp"),
    default_portscope_persistent(true),
    default_portscope_localport("/yarpscope"),
    default_plot_gridx(-1),
    default_plot_gridy(-1),
    default_plot_hspan(1),
    default_plot_vspan(1),
    default_plot_minval(-100.),
    default_plot_maxval(100.),
    default_plot_size(201),
    default_plot_bgcolor("LightSlateGrey"),
    default_plot_autorescale(false),
    default_plot_realtime(false),
    default_plot_triggermode(false),
    default_plot_title(""),
    default_graph_size(1),
    default_graph_type("lines"),
    default_graph_color("black"),
    default_graph_title(""),
    portscope_rows(default_portscope_rows),
    portscope_columns(default_portscope_columns),
    portscope_carrier(default_portscope_carrier),
    portscope_persistent(default_portscope_persistent)
{
}
