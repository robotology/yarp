#include "qtyarpscope.h"
#include <yarp/os/ResourceFinder.h>
#include "simpleloader.h"
#include "xmlloader.h"

#define PADDING         25
#define GRIDSPACING     25
#define SCALEWIDTH      25

QtYARPScope::QtYARPScope(QQuickItem *parent):
    QQuickPaintedItem(parent)
{

    setFlag(ItemHasContents, true);

    currentSelectedPlotter = NULL;
    loader = NULL;
    plotManager = PlotManager::instance();

    setAcceptedMouseButtons(Qt::AllButtons);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);

    connect(this, SIGNAL(widthChanged()), this, SLOT(updateCustomPlotSize()) );
    connect(this, SIGNAL(heightChanged()), this, SLOT(updateCustomPlotSize()));
    connect(plotManager,SIGNAL(requestRepaint()),this,SLOT(onRepaint()),Qt::QueuedConnection);

}

QtYARPScope::~QtYARPScope()
{
    if(loader){
        delete loader;
    }
}

bool QtYARPScope::parseParameters(QStringList params)
{
    // Setup resource finder
    yarp::os::ResourceFinder rf;
    rf.setVerbose();
    // TODO Read default values from yarpscope.ini
    rf.setDefaultConfigFile("yarpscope.ini");

    // Transform Qt Params array in standard argc & argv
    int c = params.count();
    char **v;
    v = (char**)malloc(sizeof(char*) * c);
    for(int i=0;i<params.count();i++){
        v[i] = (char*)malloc(sizeof(char) * params.at(i).length()+1);
        strcpy(v[i],params.at(i).toLatin1().data());
    }
    if(!rf.configure(c, v)){
        usage();
        return false;
    }

    qDebug("%s",rf.toString().data());
    // Read command line options
    yarp::os::Property options;
    //options.fromString(rf.toString());
    options.fromCommand(c,v,false);

    if (options.check("help")) {
        usage();
        return false;
    }

    //Yarp network initialization
    if (!yarp.checkNetwork()) {
        qFatal("Cannot connect to yarp network");
        return false;
    }

//********************** Deprecated options
    // local
    if (options.check("local")) {
        qWarning() << "--local option is deprecated. YarpScope now uses \"${YARP_PORT_PREFIX}/YarpScope/${REMOTE_PORT_NAME}\"";
    }

    // rows
    if (options.check("rows")) {
        qWarning() << "--rows option is deprecated. Use XML mode if you need more than one plot in a single window\"";
    }

    // cols
    if (options.check("cols")) {
        qWarning() << "--cols option is deprecated. Use XML mode if you need more than one plot in a single window\"";
    }
//********************************************
//************************* Generic options
    int interval;
    // title
    if (rf.find("title").isString()) {
        setWindowTitle(QString("%1").arg(rf.find("title").asString().data()));
    }
    // position
    if (rf.find("x").isInt() && rf.find("y").isInt()) {
        setWindowPosition(rf.find("x").asInt(), rf.find("y").asInt());
    }
    // size
    if (rf.find("dx").isInt() && rf.find("dy").isInt()) {
        setWindowSize(rf.find("dx").asInt(), rf.find("dy").asInt());
    }
    // interval
    if (rf.find("interval").isInt()) {
        interval = rf.find("interval").asInt();
    }else{
        interval = 50;
    }
//*******************************************


    bool ok;
    if (options.check("xml")) {
// XML Mode Options
        const yarp::os::Value &xmlValue = options.find("xml");
        const yarp::os::ConstString &filename = rf.findFile(xmlValue.toString().c_str());
        QString f = QString("%1").arg(filename.data());
        loader = new XmlLoader(f,plotManager,this);
        qDebug("Loading file %s",filename.c_str());
    } else {
// Command Line Mode Options
        qDebug("Loading from command line");
        loader = new SimpleLoader(&options,plotManager, &ok,this);
        if (!ok) {
            usage();
            exit(1);
        }
    }
    plotManager->setInterval(interval);
    intervalLoaded(interval);


    updateCustomPlotSize();

    return true;
}

QObject* QtYARPScope::getPlotManager()
{
    return plotManager;
}

void QtYARPScope::playPressed(int check)
{
    plotManager->playPressed(check);
}

void QtYARPScope::clear()
{
    plotManager->clear();
}

void QtYARPScope::changeInterval(int interval)
{
    plotManager->setInterval(interval);
}

void QtYARPScope::rescale()
{
    plotManager->rescale();
}

void QtYARPScope::usage() {
    qDebug("Usage: yarpscope [OPTIONS]\n");

    qDebug("OPTIONS:");
    qDebug(" --help                 Print this help and exit.\n");

    qDebug(" --title [string]       Title of the window (default \"Yarp Port Scope\")");
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
    qDebug(" --type [...]           Graph type(s). Accepted values are \"points\", \"lines\" and \"bars\" (default = \"lines\")");
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


void QtYARPScope::onRepaint()
{

    update();
}


void QtYARPScope::paint(QPainter *painter)
{
	if(!loader){
		return;
	}
    int rows = loader->portscope_rows;
    int cols = loader->portscope_columns;
    int w = painter->device()->width();
    int h = painter->device()->height();

    for (int i=0; i<plotManager->getPlotters()->count();i++)
    {
        painter->beginNativePainting(); // Workaround to flush the painter
        Plotter *plotter = (Plotter*)plotManager->getPlotters()->at(i);

        int hSpan = plotter->hspan;
        int vSpan = plotter->vspan;
        int plotterWidth = (w/cols) * hSpan;
        int plotterHeight = (h/rows) * vSpan;

        QPixmap picture( plotter->customPlot.size() );

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





void QtYARPScope::wheelEvent(QWheelEvent* event)
{
    routeMouseEvents( event );
}
void QtYARPScope::mousePressEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void QtYARPScope::mouseReleaseEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void QtYARPScope::mouseMoveEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void QtYARPScope::mouseDoubleClickEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void QtYARPScope::graphClicked( QCPAbstractPlottable* plottable )
{
    //qDebug() << Q_FUNC_INFO << QString( "Clicked on graph '%1 " ).arg( plottable->name() );
}

void QtYARPScope::routeMouseEvents( QMouseEvent* event )
{
    int x = event->x();
    int y = event->y();

    for (int i=0; i<plotManager->getPlotters()->count();i++)
    {
        Plotter *plotter = (Plotter*)plotManager->getPlotters()->at(i);
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

void QtYARPScope::routeMouseEvents( QWheelEvent* event )
{
    int x = event->x();
    int y = event->y();

    for (int i=0; i<plotManager->getPlotters()->count();i++)
    {
        Plotter *plotter = (Plotter*)plotManager->getPlotters()->at(i);
        QRectF r = plotter->paintRectGeometry;

        if(r.contains(x,y)){
            QPoint pos = QPoint(x - r.x(), y - r.y());
            QWheelEvent* newEvent = new QWheelEvent(pos, event->angleDelta().y(), event->buttons(), event->modifiers() );
            QCoreApplication::postEvent( &plotter->customPlot, newEvent );
            update();
            break;
        }
    }
}

void QtYARPScope::updateCustomPlotSize()
{

    if(loader == NULL){
        return;
    }
    int w = width();
    int h = height();
    int rows = loader->portscope_rows;
    int cols = loader->portscope_columns;

    int plottersCount = plotManager->getPlotters()->count();
    for(int i=0; i<plottersCount; i++){
        Plotter * plotter = (Plotter*)plotManager->getPlotters()->at(i);

        int hSpan = plotter->hspan;
        int vSpan = plotter->vspan;
        int plotterWidth = (w/cols) * hSpan;
        int plotterHeight = (h/rows) * vSpan;

        plotter->customPlot.setGeometry(0,0,plotterWidth,plotterHeight);
    }


}

void QtYARPScope::onCustomReplot()
{
    update();
}
