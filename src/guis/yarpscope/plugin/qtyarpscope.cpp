/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "qtyarpscope.h"
#include <yarp/os/ResourceFinder.h>
#include "simpleloader.h"
#include "xmlloader.h"
#include <yarp/os/Time.h>

#define PADDING         25
#define GRIDSPACING     25
#define SCALEWIDTH      25

QtYARPScope::QtYARPScope(QQuickItem *parent):
    QQuickPaintedItem(parent),
    i(0),
    yarp(yarp::os::YARP_CLOCK_SYSTEM),
    loader(nullptr),
    plotManager(PlotManager::instance()),
    topLevel(nullptr),
    bPressed(false),
    currentSelectedPlotter(nullptr)
{
    setFlag(ItemHasContents, true);

    setAcceptedMouseButtons(Qt::AllButtons);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);

    connect(this, SIGNAL(widthChanged()), this, SLOT(updateCustomPlotSize()) );
    connect(this, SIGNAL(heightChanged()), this, SLOT(updateCustomPlotSize()));

}

QtYARPScope::~QtYARPScope()
{
    playPressed(false);
    if (plotManager)
    {
        for (auto pltr : *(plotManager->getPlotters()))
        {
            if (pltr)
            {
                for (auto graph : static_cast<Plotter*> (pltr)->graphList)
                {
                    if (graph)
                    {
                        static_cast<Graph*> (graph)->getConnection()->freeResources();
                    }
                }
            }
        }
    }

    if(loader){
        delete loader;
    }
}
/*! \brief parse the parameters received from the main container in QstringList form
    \param params the parameter list
*/
bool QtYARPScope::parseParameters(QStringList params)
{
    //YARP network initialization
    if (!yarp.checkNetwork()) {
        qCritical("Cannot connect to yarp network");
        return false;
    }
    else
    {
        connect(plotManager,SIGNAL(requestRepaint()),this,SLOT(onRepaint()),Qt::QueuedConnection);
    }
    // Setup resource finder
    yarp::os::ResourceFinder rf;
    // TODO Read default values from yarpscope.ini
    rf.setDefaultConfigFile("yarpscope.ini");
    rf.setDefaultContext("yarpscope");

    // Transform Qt Params array in standard argc & argv
    int c = params.count();
    char **v;
    v = (char**)malloc(sizeof(char*) * c);
    for(int i=0;i<params.count();i++){
        v[i] = strdup(params.at(i).toLatin1().data());
    }

    if(!rf.configure(c, v)){
        usage();
        for(int i=0;i<params.count();i++) {
            free(v[i]);
        }
        free(v);
        return false;
    }

    qDebug("%s",rf.toString().data());

    if (rf.check("help")) {
        usage();
        for(int i=0;i<params.count();i++) {
            free(v[i]);
        }
        free(v);
        return false;
    }

    for(int i=0;i<params.count();i++) {
        free(v[i]);
    }
    free(v);

//********************** Deprecated options
    // local
    if (rf.check("local")) {
        qWarning() << "--local option is deprecated. YARPScope now uses \"${YARP_PORT_PREFIX}/YARPScope/${REMOTE_PORT_NAME}\"";
    }

    // rows
    if (rf.check("rows")) {
        qWarning() << "--rows option is deprecated. Use XML mode if you need more than one plot in a single window\"";
    }

    // cols
    if (rf.check("cols")) {
        qWarning() << "--cols option is deprecated. Use XML mode if you need more than one plot in a single window\"";
    }
//********************************************
//************************* Generic options
    int interval;
    // title
    if (rf.find("title").isString()) {
        emit setWindowTitle(QString("%1").arg(rf.find("title").asString().data()));
    }
    // position
    if (rf.find("x").isInt32() && rf.find("y").isInt32()) {
        emit setWindowPosition(rf.find("x").asInt32(), rf.find("y").asInt32());
    }
    // size
    if (rf.find("dx").isInt32() && rf.find("dy").isInt32()) {
        emit setWindowSize(rf.find("dx").asInt32(), rf.find("dy").asInt32());
    }
    // interval
    if (rf.find("interval").isInt32()) {
        interval = rf.find("interval").asInt32();
    }else{
        interval = 50;
    }
//*******************************************

    bool ok;
    if (rf.check("xml")) {
// XML Mode Options
        const std::string &filename = rf.findFile("xml");
        QString f = QString("%1").arg(filename.data());
        loader = new XmlLoader(f,plotManager,this);
        qDebug("Loading file %s",filename.c_str());
    } else {
// Command Line Mode Options
        qDebug("Loading from command line");
        loader = new SimpleLoader(&rf,plotManager, &ok,this);
        if (!ok) {
            usage();
            exit(1);
        }
    }
    plotManager->setInterval(interval);
    emit intervalLoaded(interval);


    updateCustomPlotSize();

    return true;
}

/*! \brief plays or pauses the data flow
    \param check
*/
void QtYARPScope::playPressed(int check)
{
    plotManager->playPressed(check);
}

/*! \brief clears the data in the current window*/
void QtYARPScope::clear()
{
    plotManager->clear();
}

/*! \brief changes the refresh interval
    \param interval the interval
*/
void QtYARPScope::changeInterval(int interval)
{
    plotManager->setInterval(interval);
}

/*! \brief rescales the graphs in order to contains the maximum and
 * minimum value visible in the window
*/
void QtYARPScope::rescale()
{
    plotManager->rescale();
}

/*! \brief Prints the help menu.*/
void QtYARPScope::usage() {
    qDebug("Usage: yarpscope [OPTIONS]\n");

    qDebug("OPTIONS:");
    qDebug(" --help                 Print this help and exit.\n");

    qDebug(" --title [string]       Title of the window (default \"YARP Port Scope\")");
    qDebug(" --x [uint]             Initial X position of the window.");
    qDebug(" --y [uint]             Initial Y position of the window.");
    qDebug(" --dx [uint]            Initial width of the window.");
    qDebug(" --dy [uint]            Initial height of the window.\n");

    qDebug(" --interval [int]       Initial refresh interval in milliseconds. (default = 50ms)\n");

    qDebug("XML MODE:");
    qDebug(" --xml [path]           Path to the xml with the description of the scene (all the");
    qDebug("                        \"simple mode\" options are discarded).\n");

    qDebug("SIMPLE MODE (single remote):");
    qDebug(" --remote [string]      Remote port to connect to.");
    qDebug(" --carrier [string]     YARP Carrier used for connections (default \"mcast\")");
    qDebug(" --persistent,          Make normal or persistent connections (default persistent)");
    qDebug("   --no-persistent");
    qDebug(" --index [...]          Index(es) of the vector to plot.");
    qDebug("                        It can be an [uint] or an array of [uint]s");
    qDebug(" --plot_title [string]  Plot title (default = remote)");
    qDebug(" --min [float]          Minimum value for the X axis (default -100)");
    qDebug(" --max [float]          Maximum value for the X axis (default 100)");
    qDebug(" --size [uint]          Plot size (Number of samples to plot) (default 201)");
    qDebug(" --bgcolor [string]     Background color.");
//    qDebug(" --autorescale          Rescale plot automatically.");
//    qDebug(" --realtime             Use real time mode.");
//    qDebug(" --triggermode          Use trigger mode.");
//    qDebug(" --graph_title [...]    Graph title(s) (used in legend).");
//    qDebug("                        Depending on index it must be a [string] or an array of [string]s.");
    qDebug(" --color [...]          Graph color(s).");
    qDebug("                        Depending on index it must be a [string] or an array of [string]s.");
    qDebug(R"( --type [...]           Graph type(s). Accepted values are "points", "lines" and "bars" (default = "lines"))");
    qDebug("                        Depending on index it must be a [string] or an array of [string]s.");
    qDebug(" --graph_size [...]     Graph size(s) (thickness of the points) (default = 1)");
    qDebug("                        Depending on index it must be a [uint] or an array of [uint]s.\n");


// These options are here to give a hint to the user about how these
// options from the old qt3 portscope are supposed to be replaced.
    qDebug("LEGACY OPTIONS (deprecated and unused):");
    qDebug(" --local [string]       Use YARP_PORT_PREFIX environment variable to modify default value.");
    qDebug(" --rows [uint]          Only one plot is supported from command line. Use XML mode instead.");
    qDebug(" --cols [uint]          Only one plot is supported from command line. Use XML mode instead.");
}

/*! \brief called when the graphs must be repainted*/
void QtYARPScope::onRepaint()
{
    update();
}

/*! \brief paint method.
    In this method each Plotter is grabbed from its own QCustomPlot Widget
    to a Pixmap and then drawed with the QPainter primitive drawPixmap
*/
void QtYARPScope::paint(QPainter *painter)
{
    if(!loader) {
        return;
    }
    int rows = loader->portscope_rows;
    int cols = loader->portscope_columns;
    int w = painter->device()->width();
    int h = painter->device()->height();

    for (int i=0; i<plotManager->getPlotters()->count();i++)
    {
        painter->beginNativePainting(); // Workaround to flush the painter
        auto* plotter = (Plotter*)plotManager->getPlotters()->at(i);

        int hSpan = plotter->hspan;
        int vSpan = plotter->vspan;
        int plotterWidth = (w/cols) * hSpan;
        int plotterHeight = (h/rows) * vSpan;

        QPixmap picture(QSize(plotter->customPlot.width() * plotter->customPlot.devicePixelRatio(),
                              plotter->customPlot.height() * plotter->customPlot.devicePixelRatio()));

        QCPPainter qcpPainter( &picture );
        plotter->customPlot.toPainter( &qcpPainter );
        QRectF r = QRectF(plotter->gridx * plotterWidth/hSpan,
                          plotter->gridy * plotterHeight/vSpan,
                          picture.rect().width(),picture.rect().height());
        plotter->setPaintGeometry(r);
        painter->drawPixmap(r,picture,QRectF(0,0,plotterWidth,plotterHeight) );

        painter->endNativePainting();
    }

}




/*! \brief the wheel mouse event
    \param event the event
*/
void QtYARPScope::wheelEvent(QWheelEvent* event)
{
    routeMouseEvents( event );
}

/*! \brief the mouse press event
    \param event the event
*/
void QtYARPScope::mousePressEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

/*! \brief the mouse release event
    \param event the event
*/
void QtYARPScope::mouseReleaseEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

/*! \brief the mouse move event
    \param event the event
*/
void QtYARPScope::mouseMoveEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

/*! \brief the mouse double click event
    \param event the event
*/
void QtYARPScope::mouseDoubleClickEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void QtYARPScope::graphClicked( QCPAbstractPlottable* plottable )
{
    //qDebug() << Q_FUNC_INFO << QString( "Clicked on graph '%1 " ).arg( plottable->name() );
}

/*! \brief this function is used to route the mouse events on the core plugin
 *  to the relative QCustomPlot.
 *  This method works fairly well but has some limitations on dragging
    \param event the event
*/
void QtYARPScope::routeMouseEvents( QMouseEvent* event )
{
    int x = event->x();
    int y = event->y();

    for (int i=0; i<plotManager->getPlotters()->count();i++)
    {
        auto* plotter = (Plotter*)plotManager->getPlotters()->at(i);
        QRectF r = plotter->paintRectGeometry;

        if(r.contains(x,y)){
            QPoint pos = QPoint(x - r.x(), y - r.y());
            QMouseEvent* newEvent = new QMouseEvent( event->type(), pos, event->button(), event->buttons(), event->modifiers() );
            QCoreApplication::postEvent( &plotter->customPlot, newEvent,Qt::HighEventPriority );
            update();
            break;
        }
    }
}

/*! \brief this function is used to route the wheel mouse events on the core plugin
 *  to the relative QCustomPlot.
    \param event the event
*/
void QtYARPScope::routeMouseEvents( QWheelEvent* event )
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    int x = event->position().x();
    int y = event->position().y();
#else
    int x = event->x();
    int y = event->y();
#endif

    for (int i=0; i<plotManager->getPlotters()->count();i++)
    {
        auto* plotter = (Plotter*)plotManager->getPlotters()->at(i);
        QRectF r = plotter->paintRectGeometry;

        if(r.contains(x,y)){
            QPoint pos = QPoint(x - r.x(), y - r.y());
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            QWheelEvent* newEvent = new QWheelEvent(pos,
                                                    event->globalPosition(),
                                                    event->pixelDelta(),
                                                    event->angleDelta(),
                                                    event->buttons(),
                                                    event->modifiers(),
                                                    event->phase(),
                                                    event->inverted(),
                                                    event->source());
#else
            QWheelEvent* newEvent = new QWheelEvent(pos, event->angleDelta().y(), event->buttons(), event->modifiers() );
#endif
            QCoreApplication::postEvent( &plotter->customPlot, newEvent );
            update();
            break;
        }
    }
}

/*! \brief this function is used to update the size of the QCustomPlots
 *  when changing the size of the main window
*/
void QtYARPScope::updateCustomPlotSize()
{

    if(loader == nullptr){
        return;
    }
    int w = width();
    int h = height();
    int rows = loader->portscope_rows;
    int cols = loader->portscope_columns;

    int plottersCount = plotManager->getPlotters()->count();
    for(int i=0; i<plottersCount; i++){
        auto* plotter = (Plotter*)plotManager->getPlotters()->at(i);

        int hSpan = plotter->hspan;
        int vSpan = plotter->vspan;
        int plotterWidth = (w/cols) * hSpan;
        int plotterHeight = (h/rows) * vSpan;

        plotter->customPlot.setGeometry(0,0,plotterWidth,plotterHeight);
    }
}
