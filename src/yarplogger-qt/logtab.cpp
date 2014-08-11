#include "logtab.h"
#include "ui_logtab.h"

LogTab::LogTab(yarp::os::YarprunLogger::LoggerEngine*  _theLogger, std::string _portName, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LogTab)
{ 
    theLogger= _theLogger;
    portName =_portName;
    displayTimestamp=false;
    ui->setupUi(this);
    model_logs = new QStandardItemModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model_logs);
    proxyModel->setFilterKeyColumn(-1); 
    ui->listView->setModel(proxyModel);
    //ui->listView->setModel(model_logs);

    ui->listView->verticalHeader()->setVisible(false);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);


    logTimer = new QTimer(this);
    connect(logTimer, SIGNAL(timeout()), this, SLOT(updateLog()));
    logTimer->start(500);

    model_logs->setHorizontalHeaderItem(0,new QStandardItem("timestamp"));
    model_logs->setHorizontalHeaderItem(1,new QStandardItem("level"));
    model_logs->setHorizontalHeaderItem(2,new QStandardItem("message"));
}

LogTab::~LogTab()
{
    delete ui;
}

void LogTab::updateLog()
{
    std::list<yarp::os::YarprunLogger::MessageEntry> messages;
    this->theLogger->get_messages_by_port_complete(portName,messages);
    
    std::list<yarp::os::YarprunLogger::MessageEntry>::iterator it;
    model_logs->clear(); //@@@@@@@@@@@@TO BE REMOVED LATER
    QStandardItem *rootNode = model_logs->invisibleRootItem();
    for (it=messages.begin(); it!=messages.end(); it++)
    {
        QList<QStandardItem *> rowItem;
        std:: string error_level = "INFO"; //it->level;
        rowItem << new QStandardItem(it->timestamp.c_str()) << new QStandardItem(error_level.c_str()) << new QStandardItem(it->text.c_str());
        rootNode->appendRow(rowItem);
    }
}
