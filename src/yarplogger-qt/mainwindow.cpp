#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logtab.h"
#include "ui_logtab.h"
#include <QFile>
#include <QTextStream>
#include <yarp/os/YarprunLogger.h>
#include <QAbstractItemModel>
#include <QStandardItemModel>

void MainWindow::updateMain()
{
    /*std::list<yarp::os::YarprunLogger::MessageEntry> messages;
    this->theLogger->get_messages(messages);
    QStringList List;
    std::list<yarp::os::YarprunLogger::MessageEntry>::iterator it;
    for (it=messages.begin(); it!=messages.end(); it++)
    {
        List << it->text.c_str();
    }
    model->setStringList(List);*/
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    theLogger = new yarp::os::YarprunLogger::LoggerEngine("/logger");

    ui->setupUi(this);
        loadTextFile();

    model_yarprunports = new QStringListModel(this);
    
    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(500);

    ui->stopLogger->setEnabled(false);
    ui->refreshLogger->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadTextFile()
{
    /*
    QFile inputFile("c:\\ptcsetup.log");
    inputFile.open(QIODevice::ReadOnly);

    QTextStream in(&inputFile);
    QString line = in.readAll();
    inputFile.close();

    ui->textEdit->setPlainText(line);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
    */
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    if (checked)
    {
        QRegExp regExp("*f*", Qt::CaseInsensitive, QRegExp::Wildcard);
/////        proxyModel->setFilterRegExp(regExp);
    }
    else
    {
        QRegExp regExp("*", Qt::CaseInsensitive, QRegExp::Wildcard);
  /////      proxyModel->setFilterRegExp(regExp);
    }
}

void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    QString filter = "*";
    filter.append(arg1);
    filter.append("*");
    QRegExp regExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);

    int currentTab = ui->logtabs->currentIndex();
    LogTab* logtab = ui->logtabs->currentWidget()->findChild<LogTab*>("logtab");

    logtab->proxyModel->setFilterRegExp(regExp);
}

void MainWindow::on_startLogger_clicked()
{
     this->theLogger->start_logging();

     ui->startLogger->setEnabled(false);
     ui->stopLogger->setEnabled(true);
     ui->refreshLogger->setEnabled(true);
}


void MainWindow::on_stopLogger_clicked()
{
     this->theLogger->stop_logging();

     ui->stopLogger->setEnabled(false);
     ui->startLogger->setEnabled(true);
     ui->refreshLogger->setEnabled(false);
}

void MainWindow::on_refreshLogger_clicked()
{
    std::list<std::string> ports;
    theLogger->discover(ports);
    theLogger->connect(ports);

    QStringList List;
    std::list<std::string>::iterator it;
    for (it=ports.begin(); it!=ports.end(); it++)
    {
        List << (*it).c_str() ;
    }
    model_yarprunports->setStringList(List);
    ui->treeView->setModel(model_yarprunports);
    /*QStringList l1;
    l1 << "yarprun ports";
    ui->treeView->setHorizontalHeaderLabels (l1);
    model_yarprunports->setHeaderData(0);*/

    
    /*
    std::list<std::string>::iterator it;
    for (it=ports.begin(); it!=ports.end(); it++)
    {

    }
    QStandardItemModel* model = new QStandardItemModel();
    QStandardItem* item0 = new QStandardItem(QIcon("test.png"), "1 first item");
    QStandardItem* item1 = new QStandardItem(QIcon("test.png"), "2 second item");
    QStandardItem* item3 = new QStandardItem(QIcon("test.png"), "3 third item");
    QStandardItem* item4 = new QStandardItem("4 forth item");
    model->appendRow(item0);
    item0->appendRow(item3);
    item0->appendRow(item4);
    model->appendRow(item1);
    ui->treeView->setModel(model);
    */
}

void MainWindow::on_logtabs_tabCloseRequested(int index)
{
    delete ui->logtabs->widget(index);
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QTabWidget* tab = new QTabWidget(this);
    QString tabname = model_yarprunports->data(index,Qt::DisplayRole).toString();
    LogTab* tmpLogTab = new LogTab(theLogger, tabname.toStdString(), tab);
    tmpLogTab->setObjectName("logtab");

    int exists = -1;
    for (int i=0; i<ui->logtabs->count(); i++)
        if (ui->logtabs->tabText(i) == tabname) exists = i;

    if (exists>=0)
    {
        ui->logtabs->setCurrentIndex(exists);
    }
    else
    {
        int newtab_index = ui->logtabs->addTab(tab, tabname);
        ui->logtabs->setCurrentIndex(newtab_index);
    }
}
