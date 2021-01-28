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

#ifndef SIMPLELOADER_H
#define SIMPLELOADER_H

#include <QObject>
#include "yarp/os/ResourceFinder.h"
#include "plotmanager.h"
#include "genericloader.h"

/*! \class SimpleLoader
    \brief Reads a configuration from a xml file
*/
class SimpleLoader : public GenericLoader
{
    Q_OBJECT
public:
    explicit SimpleLoader(/* FIXME const */ yarp::os::ResourceFinder *options, PlotManager *plotManager,bool *ok, QObject *parent = 0);

private:
    PlotManager *plotManager;

    QString plot_bgcolor;
    int plot_size;
    float plot_minval;
    float plot_maxval;
    bool plot_autorescale;
    bool plot_realtime;
    bool plot_triggermode;
    QString plot_title;

    QString graph_remote;
    int graph_index;
    QString graph_localport;
    QString graph_title;
    QString graph_color;
    QString graph_type;
    int graph_size;
};

#endif // SIMPLELOADER_H
