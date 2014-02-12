#ifndef XMLLOADER_H
#define XMLLOADER_H

#include "genericloader.h"

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
