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

#ifndef XMLLOADER_H
#define XMLLOADER_H

#include "genericloader.h"

/*! \class XmlLoader
    \brief Reads a configuration from a xml file
*/
class XmlLoader : public GenericLoader
{
    Q_OBJECT
public:
    XmlLoader(QString fileName, PlotManager *plotManager, QObject *parent = 0);

private:
    PlotManager *plotManager;
    Plotter *plotter;
};

#endif // XMLLOADER_H
