/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
