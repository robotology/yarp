#include "logtab.h"
#include "ui_logtab.h"

LogTab::LogTab(yarp::os::YarprunLogger::LoggerEngine*  _theLogger, std::string _portName, QWidget *parent, int refreshRate) :
    QFrame(parent),
    ui(new Ui::LogTab)
{ 
    theLogger= _theLogger;
    portName =_portName;
    displayTimestamp_enabled=true;
    displayColors_enabled=true;
    displayGrid_enabled=true;
    displayErrorLevel_enabled=true;
    ui->setupUi(this);
    model_logs = new QStandardItemModel();
    proxyModelButtons = new QSortFilterProxyModel(this);
    proxyModelSearch = new QSortFilterProxyModel(this);
#define USE_FILTERS 1
#if USE_FILTERS
    proxyModelButtons->setSourceModel(model_logs);
    proxyModelSearch->setSourceModel(proxyModelButtons);
    proxyModelSearch->setFilterKeyColumn(-1); 
    ui->listView->setModel(proxyModelSearch);
#else
    ui->listView->setModel(model_logs);
#endif
    ui->listView->verticalHeader()->setVisible(false);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listView->setAutoScroll(true);

    logTimer = new QTimer(this);
    connect(logTimer, SIGNAL(timeout()), this, SLOT(updateLog()));
    logTimer->start(refreshRate);

    model_logs->setHorizontalHeaderItem(0,new QStandardItem("timestamp"));
    model_logs->setHorizontalHeaderItem(1,new QStandardItem("level"));
    model_logs->setHorizontalHeaderItem(2,new QStandardItem("message"));
    ui->listView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listView->verticalHeader()->setDefaultSectionSize(20);
    
    updateLog(true);
}

LogTab::~LogTab()
{
    delete ui;
}

void LogTab::clear_model_logs()
{
    mutex.lock();
    if (model_logs) model_logs->removeRows(0,model_logs->rowCount());
    mutex.unlock();
}

void LogTab::updateLog(bool from_beginning)
{
    mutex.lock();
    std::list<yarp::os::YarprunLogger::MessageEntry> messages;
    this->theLogger->get_messages_by_port_complete(portName,messages, from_beginning);
    int size_messages= messages.size();
    std::list<yarp::os::YarprunLogger::MessageEntry>::iterator it;
    QStandardItem *rootNode = model_logs->invisibleRootItem();
    for (it=messages.begin(); it!=messages.end(); it++)
    {
        QList<QStandardItem *> rowItem;
        QColor rowcolor = QColor(Qt::white);
        std:: string error_level;
        if      (it->level==yarp::os::YarprunLogger::LOGLEVEL_ERROR)     { rowcolor = QColor(Qt::red);    error_level=ERROR_STRING;}
        else if (it->level==yarp::os::YarprunLogger::LOGLEVEL_WARNING)   { rowcolor = QColor(Qt::yellow); error_level=WARNING_STRING; }
        else if (it->level==yarp::os::YarprunLogger::LOGLEVEL_INFO)      { rowcolor = QColor(Qt::green);  error_level=INFO_STRING; }
        else if (it->level==yarp::os::YarprunLogger::LOGLEVEL_DEBUG)     { rowcolor = QColor(Qt::blue);   error_level=DEBUG_STRING;}
        else if (it->level==yarp::os::YarprunLogger::LOGLEVEL_UNDEFINED) { rowcolor = QColor(Qt::white);  error_level="";     }
        else                                                             { rowcolor = QColor(Qt::white);  error_level="";     }

        rowItem << new QStandardItem(it->timestamp.c_str()) << new QStandardItem(error_level.c_str()) << new QStandardItem(it->text.c_str());

        if (displayColors_enabled)
        {
            for (QList<QStandardItem *>::iterator col_it = rowItem.begin(); col_it != rowItem.end(); col_it++)
            {
                (*col_it)->setBackground(rowcolor);
            }
        }
        rootNode->appendRow(rowItem);
    }
    ui->listView->setColumnHidden(0,!displayTimestamp_enabled);
    ui->listView->setColumnHidden(1,!displayErrorLevel_enabled);
    ui->listView->setShowGrid(displayGrid_enabled);
    mutex.unlock();
}

void LogTab::displayTimestamp  (bool enabled)
{
    displayTimestamp_enabled=enabled;
    ui->listView->setColumnHidden(0,!displayTimestamp_enabled);
}

void LogTab::displayErrorLevel (bool enabled)
{
    displayErrorLevel_enabled=enabled;
    ui->listView->setColumnHidden(1,!displayErrorLevel_enabled);
}

void LogTab::displayColors     (bool enabled)
{
    displayColors_enabled=enabled;
    //ui->listView->
}

void LogTab::displayGrid       (bool enabled)
{
    displayGrid_enabled=enabled;
    ui->listView->setShowGrid(displayGrid_enabled);
}