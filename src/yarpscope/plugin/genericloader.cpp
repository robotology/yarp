/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
