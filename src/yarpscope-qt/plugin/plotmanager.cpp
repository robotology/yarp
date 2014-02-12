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

PlotManager * PlotManager::instance()
{
    if(self == NULL){
        return new PlotManager();
    }
    return self;
}

QList<QObject *>* PlotManager::getPlotters()
{
    return &plotterList;
}

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



void PlotManager::setInterval(int interval)
{

    timer.setInterval(interval);
    if(!timer.isActive())
        timer.start();
}


void PlotManager::onTimeout()
{
    int c = plotterList.count();
    for(int i=0;i<c;i++){
        ((Plotter*)plotterList.at(i))->onTimeout();
    }
    requestRepaint();
}

void PlotManager::playPressed(bool check)
{
    if(!check){
        timer.stop();
    }else{
        timer.start();
    }
}


void PlotManager::clear()
{
    for(int i=0;i<plotterList.count();i++){
        ((Plotter*)plotterList.at(i))->clear();
    }
}

void PlotManager::rescale()
{
    for(int i=0;i<plotterList.count();i++){
        ((Plotter*)plotterList.at(i))->rescale();
    }
}
