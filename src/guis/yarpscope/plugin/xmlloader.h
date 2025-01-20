/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
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
