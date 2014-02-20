/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "plotmanager.h"

PlotManager *PlotManager::self = NULL;

PlotManager::PlotManager(QObject *parent) :
    QObject(parent)
{
    self = this;
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimeout()),Qt::DirectConnection);
}

PlotManager::~PlotManager()
{
    if(timer.isActive()){
        timer.stop();
    }
}

/*! \brief Returns an instance of the class (Singleton). */
PlotManager * PlotManager::instance()
{
    if(self == NULL){
        return new PlotManager();
    }
    return self;
}

/*! \brief Returns a list of all plotters. */
QList<QObject *>* PlotManager::getPlotters()
{
    return &plotterList;
}

/*! \brief Adds a new Plotter
 *  \param title the title of the plotter
 *  \param gridx the x pos in the grid
 *  \param gridy the y pos in the grid
 *  \param hspan the horizonatl span
 *  \param vspan the vertical span
 *  \param minval the minimum scale value
 *  \param maxval the maximium scale value
 *  \param size number of sample of datas in plotter
 *  \param bgcolor the background color of the plotter
 *  \param autorescale not used
*/
Plotter * PlotManager::addPlot(const QString &title,
                int gridx,
                int gridy,
                int hspan,
                int vspan,
                float minval,
                float maxval,
                int size,
                const QString &bgcolor,
                bool autorescale)
{
    Plotter *plotter = new Plotter(title,gridx,gridy,hspan,vspan,minval,maxval,size,bgcolor,autorescale,this);
    plotterList.append(plotter);
    plottersChanged();
    plotter->plotSampleSizeChanged();

    return plotter;

}


/*! \brief Sets the refresh interval
    \param interval the interval
*/
void PlotManager::setInterval(int interval)
{

    timer.setInterval(interval);
    if(!timer.isActive())
        timer.start();
}

/*! \brief Timeout of the refresh timer */
void PlotManager::onTimeout()
{
    int c = plotterList.count();
    for(int i=0;i<c;i++){
        ((Plotter*)plotterList.at(i))->onTimeout();
    }
    requestRepaint();
}

/*! \brief Sets the play or pause state
    \param check
*/
void PlotManager::playPressed(bool check)
{
    if(!check){
        timer.stop();
    }else{
        timer.start();
    }
}

/*! \brief Clear all plotters datas */
void PlotManager::clear()
{
    for(int i=0;i<plotterList.count();i++){
        ((Plotter*)plotterList.at(i))->clear();
    }
}

/*! \brief Rescale all plotters */
void PlotManager::rescale()
{
    for(int i=0;i<plotterList.count();i++){
        ((Plotter*)plotterList.at(i))->rescale();
    }
}
