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

#include "plotmanager.h"

PlotManager::PlotManager(QObject *parent) :
    QObject(parent)
{
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimeout()),Qt::DirectConnection);
}

PlotManager::~PlotManager()
{

    if(timer.isActive()){
        timer.stop();
    }

    /*while(!plotterList.isEmpty()){
        delete ((Plotter*)plotterList.takeLast());
    }*/
}

/*! \brief Returns an instance of the class (Singleton). */
PlotManager * PlotManager::instance()
{
    static PlotManager instance;
    return &instance;
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
    auto* plotter = new Plotter(title,gridx,gridy,hspan,vspan,minval,maxval,size,bgcolor,autorescale,this);
    plotterList.append(plotter);
    emit plottersChanged();
    emit plotter->plotSampleSizeChanged();

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
    emit requestRepaint();
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
