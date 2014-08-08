#include "logtab.h"
#include "ui_logtab.h"

LogTab::LogTab(yarp::os::YarprunLogger::LoggerEngine*  _theLogger, std::string _portName, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LogTab)
{ 
    theLogger= _theLogger;
    portName =_portName;
    ui->setupUi(this);
    logs       = new QStringListModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(logs);
    ui->listView->setModel(proxyModel);

    logTimer = new QTimer(this);
    connect(logTimer, SIGNAL(timeout()), this, SLOT(updateLog()));
    logTimer->start(500);
}

LogTab::~LogTab()
{
    delete ui;
}

void LogTab::updateLog()
{
    std::list<yarp::os::YarprunLogger::MessageEntry> messages;
    this->theLogger->get_messages_by_port_complete(portName,messages);
    QStringList List;
    std::list<yarp::os::YarprunLogger::MessageEntry>::iterator it;
    for (it=messages.begin(); it!=messages.end(); it++)
    {
        List << it->text.c_str();
    }
    logs->setStringList(List);
}
