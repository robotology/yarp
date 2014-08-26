#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "advanced_dialog.h"
#include "logtab.h"
#include "ui_logtab.h"
#include <QString>
#include <QMenu>
#include <QTextStream>
#include <ctime>
#include <yarp/os/YarprunLogger.h>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QSignalMapper>
#include <QFileDialog>
#include <QDateTime>

void MainWindow::updateMain()
{
    //model_yarprunports->clear();
    model_yarprunports->setHorizontalHeaderItem(0,new QStandardItem("ip"));
    model_yarprunports->setHorizontalHeaderItem(1,new QStandardItem("process"));
    model_yarprunports->setHorizontalHeaderItem(2,new QStandardItem("last heard"));
    model_yarprunports->setHorizontalHeaderItem(3,new QStandardItem("log size"));
    model_yarprunports->setHorizontalHeaderItem(4,new QStandardItem("errors"));
    model_yarprunports->setHorizontalHeaderItem(5,new QStandardItem("warnings"));
    std::list<yarp::os::YarprunLogger::LogEntryInfo> infos;
    this->theLogger->get_infos (infos);
    std::list<yarp::os::YarprunLogger::LogEntryInfo>::iterator it;

    QStandardItem *itemsRoot = model_yarprunports->invisibleRootItem();
    for (it=infos.begin(); it!=infos.end(); it++)
    {
        const size_t string_size= 50;
        char time_text[string_size];
        std::tm* tm = localtime(&it->last_update);
        if (tm)
           sprintf ( time_text,"%02d:%02d:%02d",tm->tm_hour,tm->tm_min, tm->tm_sec);
        else
           sprintf ( time_text, "no data received yet");
        
        char logsize_text[10];
        sprintf (logsize_text, "%d", it->logsize);

        char logerrors_text[10];
        sprintf (logerrors_text, "%d", it->get_number_of_errors());

        char logwarnings_text[10];
        sprintf (logwarnings_text, "%d", it->get_number_of_warnings());

//#define TREE_MODEL 1
#define IN_ROW_MODEL 1
#if IN_ROW_MODEL
        bool existing = false;
        int i=0;
        for (i=0; i<model_yarprunports->rowCount(); i++)
        {
            QStandardItem *item = model_yarprunports->item(i,1);
            if (item && item->text()==it->port_complete.c_str())
            {
                model_yarprunports->item(i,2)->setText(time_text);
                model_yarprunports->item(i,3)->setText(logsize_text);
                model_yarprunports->item(i,4)->setText(logerrors_text);
                model_yarprunports->item(i,5)->setText(logwarnings_text);

                QColor rowcolor = QColor(Qt::white);
                yarp::os::YarprunLogger::LogLevelEnum last_error = it->getLastError();
                if      (last_error==yarp::os::YarprunLogger::LOGLEVEL_ERROR)   rowcolor = QColor(Qt::red);
                else if (last_error==yarp::os::YarprunLogger::LOGLEVEL_WARNING) rowcolor = QColor(Qt::yellow);

                for (int j=0; j<model_yarprunports->columnCount(); j++)
                    model_yarprunports->item(i,j)->setBackground(rowcolor);
                
                existing = true;
                if (!tm && show_mute_ports_enabled==false) {model_yarprunports->removeRow(i);}
                break;
            }
        }
        if (existing == false)
        {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(it->ip_address.c_str()) << new QStandardItem(it->port_complete.c_str()) <<
                        new QStandardItem(time_text) << new QStandardItem(logsize_text) <<
                        new QStandardItem(logerrors_text) << new QStandardItem (logwarnings_text);
            if (show_mute_ports_enabled || (!show_mute_ports_enabled && tm)) {itemsRoot->appendRow(rowItems);}
        }

#elif TREE_MODEL
        bool level1_exists = false;
        bool level2_exists = false;
        for (int i=0; i<model_yarprunports->rowCount(); i++)
        {
            QStandardItem *item_level1 = model_yarprunports->item(i,0);
            if (item_level1 && item_level1->text() == it->port_prefix.c_str())
            {
                level1_exists = true;
                for (int j=0; j<item_level1->rowCount(); j++)
                {
                    QStandardItem *item_level2 = item_level1->child(j,0);
                    if (item_level2 && item_level1->child(j,1)->text() == it->port_complete.c_str())
                    {
                        level2_exists = true;
                        item_level1->child(j,2)->text() = time_text;
                        break;
                    }
                }
                if (level2_exists == false)
                {
                    QList<QStandardItem *> rowItems;
                    rowItems << new QStandardItem(it->port_prefix.c_str()) << new QStandardItem(it->port_complete.c_str()) << new QStandardItem(time_text);
                    item_level1->appendRow(rowItems);
                    break;
                }
            }
        }
        if (level1_exists == false)
        {
            /*QList<QStandardItem *> rowItems_l1;
            QList<QStandardItem *> rowItems_l2;
            rowItems_l1 << new QStandardItem(it->port_prefix.c_str());
            rowItems_l2 << new QStandardItem(it->port_prefix.c_str()) << new QStandardItem(it->port_complete.c_str()) << new QStandardItem(time_text);
            rowItems_l1->appendRow(rowItems_l2);
            itemsRoot->appendRow(rowItems_l1);*/
            QList<QStandardItem *> rowItems_l1;
            rowItems_l1 << new QStandardItem(it->port_prefix.c_str());
            itemsRoot->appendRow(rowItems_l1);
        }
        /*if (existing == false)
        {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(it->port_prefix.c_str()) << new QStandardItem(it->port_complete.c_str()) << new QStandardItem(time_text);
            itemsRoot->appendRow(rowItems);
        }
        bool rowfound = false;
        for (int i=0; i<model_yarprunports->rowCount(); i++)
        {
            QStandardItem *rowParent = model_yarprunports->item(i,0);
            if (rowParent && rowParent->text()==it->port_prefix.c_str())
            {
                rowParent->appendRow(rowItems);
                rowfound = true;
            }
        }
        if  (rowfound==false)
        {
            itemsRoot->appendRow(rowItems);
        }*/
#endif
    }
    ui->yarprunTreeView->setColumnWidth(0,80);
    ui->yarprunTreeView->setColumnWidth(1,230);
    ui->yarprunTreeView->setColumnWidth(2,80);
    ui->yarprunTreeView->setColumnWidth(3,50);
    ui->yarprunTreeView->setColumnWidth(4,60);
    ui->yarprunTreeView->setColumnWidth(5,60);
}

void MainWindow::on_clearLogTab_action()
{
    QVariant qvar=qobject_cast<QAction*>(sender())->property("model_row");
    if (qvar.isValid()==false)
    {
        return;
    }
    int model_row = qvar.toInt();
    QString logname = model_yarprunports->item(model_row,1)->text();
    theLogger->clear_messages_by_port_complete(logname.toStdString());
    for (int i=0; i<ui->logtabs->count(); i++)
        if (ui->logtabs->tabText(i) == logname) 
            {
                LogTab* l = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
                if (l) l->clear_model_logs();
                break;
            }
}

void MainWindow::on_saveLogTab_action()
{
    QVariant qvar=qobject_cast<QAction*>(sender())->property("model_row");
    if (qvar.isValid()==false)
    {
        return;
    }
    int model_row = qvar.toInt();
    std::string logname = model_yarprunports->item(model_row,1)->text().toStdString();
    std::string preferred_filename=logname;
    std::replace(preferred_filename.begin(), preferred_filename.end(), '/', '_');
    preferred_filename.erase(0,1);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export log to text file"),preferred_filename.c_str(), tr("Text Files (*.txt)"));
    theLogger->export_log_to_text_file(fileName.toStdString(),logname);
}

void MainWindow::ctxMenu(const QPoint &pos)
{
    QModelIndex index = ui->yarprunTreeView->indexAt(pos);
    int model_row=index.row();
    if (model_row==-1) return;

    QMenu *menu = new QMenu;
    QAction *act1 = menu->addAction(tr("Clear current log"), this, SLOT(on_clearLogTab_action()));
    QAction *act2 = menu->addAction(tr("Export current log to text file"), this, SLOT(on_saveLogTab_action()));
    act1->setProperty("model_row", model_row);
    act2->setProperty("model_row", model_row);
    menu->exec(ui->yarprunTreeView->mapToGlobal(pos));
}

MainWindow::MainWindow(yarp::os::ResourceFinder rf, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    std::string loggername = rf.check("name",yarp::os::Value("/logger")).asString();
    theLogger = new yarp::os::YarprunLogger::LoggerEngine(loggername);

    ui->setupUi(this);

    model_yarprunports = new QStandardItemModel(this);
    statusBarLabel = new QLabel;
    statusBarLabel->setText("Ready");
    statusBarLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addWidget(statusBarLabel);

    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(500);

    ui->yarprunTreeView->setModel(model_yarprunports);
    ui->yarprunTreeView->expandAll();

    connect(ui->yarprunTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

    QMenu *m=ui->menuFile;
    m->actions().at(0)->setEnabled(true);
    m->actions().at(1)->setEnabled(false);
    m->actions().at(2)->setEnabled(false);
}

MainWindow::~MainWindow()
{
    this->theLogger->stop_logging();
    if (mainTimer) {delete mainTimer; mainTimer=0;}
    if (ui)        {delete ui; ui=0;}
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

void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    QString filter = "*";
    filter.append(arg1);
    filter.append("*");
    QRegExp regExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);

    int currentTab = ui->logtabs->currentIndex();
    LogTab* logtab = ui->logtabs->currentWidget()->findChild<LogTab*>("logtab");

    if (logtab) logtab->proxyModelSearch->setFilterRegExp(regExp);
}

void MainWindow::on_logtabs_tabCloseRequested(int index)
{
    delete ui->logtabs->widget(index);
}

void MainWindow::on_yarprunTreeView_doubleClicked(const QModelIndex &index)
{
    QTabWidget* tab = new QTabWidget(this);
    int model_row=index.row();
    QString tabname = model_yarprunports->item(model_row,1)->text();
    QVBoxLayout* l= new QVBoxLayout(tab);
    LogTab* tmpLogTab = new LogTab(theLogger, tabname.toStdString(), this);
    tmpLogTab->displayTimestamp(displayTimestamps);
    tmpLogTab->displayErrorLevel(displayErrorLevel);
    tmpLogTab->displayGrid(displayGrid);
    tmpLogTab->displayColors(displayColors);
    l->addWidget(tmpLogTab);
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
    apply_button_filters(); //@@@@NOT WOKRING HERE
}

QString MainWindow::recomputeFilters()
{
    QString filter;
    bool e_debug   = this->ui->DisplayDebugEnable->isChecked();
    bool e_error   = this->ui->DisplayErrorEnable->isChecked();
    bool e_info    = this->ui->DisplayInfoEnable->isChecked();
    bool e_warning = this->ui->DisplayWarningEnable->isChecked();
    bool e_all     = this->ui->DisplayUnformattedEnable->isChecked();
    int f = 0;
    if (e_debug)   {if (f>0) filter=filter +"|"; filter = filter + "^DEBUG$";   f++;}
    if (e_error)   {if (f>0) filter=filter +"|"; filter = filter + "^ERROR$";   f++;}
    if (e_info)    {if (f>0) filter=filter +"|"; filter = filter + "^INFO$";    f++;}
    if (e_warning) {if (f>0) filter=filter +"|"; filter = filter + "^WARNING$"; f++;}
    if (e_all)     {if (f>0) filter=filter +"|"; filter = filter + "^$";        f++;}
    std::string debug = filter.toStdString();
    return filter;
}

void MainWindow::apply_button_filters()
{
    QRegExp regExp ("*", Qt::CaseInsensitive, QRegExp::RegExp);
    regExp.setPattern(recomputeFilters());
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        LogTab* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab) {
                      logtab->proxyModelButtons->setFilterRegExp(regExp);
                      logtab->proxyModelButtons->setFilterKeyColumn(1);
                    }
    }
}

void MainWindow::on_DisplayErrorEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_DisplayWarningEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_DisplayDebugEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_DisplayInfoEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_DisplayUnformattedEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_actionShow_Timestamps_toggled(bool arg1)
{
    displayTimestamps = arg1;
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        LogTab* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab) 
        {
            logtab->displayTimestamp(displayTimestamps);
        }
    }
}

void MainWindow::on_actionAbout_QtYarpLogger_triggered()
{
    QDesktopServices::openUrl(QUrl("http://wiki.icub.org/yarpdoc/qtyarplogger.html"));
}

void MainWindow::on_actionSave_Log_triggered(bool checked)
{
    QString dateformat = "yarprunlog_dd_MM_yyyy_hh_mm_ss";
    QDateTime currDate = QDateTime::currentDateTime();
    QString preferred_filename = currDate.toString ( dateformat )+".log";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to log file"),preferred_filename, tr("Log Files (*.log)"));
    theLogger->save_all_logs_to_file(fileName.toStdString());
}

void MainWindow::on_actionLoad_Log_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load log file"),"./", tr("Log Files (*.log)"));
    theLogger->load_all_logs_from_file(fileName.toStdString());
}

void MainWindow::on_actionShow_Error_Level_toggled(bool arg1)
{
    displayErrorLevel = arg1;
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        LogTab* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab) 
        {
            logtab->displayErrorLevel(displayErrorLevel);
        }
    }
}

void MainWindow::on_actionShow_Colors_toggled(bool arg1)
{
    displayColors = arg1;
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        LogTab* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab) 
        {
            logtab->displayColors(displayColors);
        }
    }
}

void MainWindow::on_actionShow_Grid_toggled(bool arg1)
{
    displayGrid = arg1;
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        LogTab* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab) 
        {
            logtab->displayGrid(displayGrid);
        }
    }
}

void MainWindow::on_actionAdvanced_triggered()
{
    QDialog* advanced = new advanced_dialog(theLogger, this);
    advanced->show();
}

void MainWindow::on_actionShow_Mute_Ports_toggled(bool arg1)
{
    show_mute_ports_enabled = arg1;
}

void MainWindow::on_actionStart_Logger_triggered()
{
    statusBarLabel->setText("Running");
    this->theLogger->start_logging();

    QMenu *m=ui->menuFile;
    m->actions().at(0)->setEnabled(false);
    m->actions().at(1)->setEnabled(true);
    m->actions().at(2)->setEnabled(true);
}

void MainWindow::on_actionStop_Logger_triggered()
{
    statusBarLabel->setText("Stopped");
    this->theLogger->stop_logging();

    QMenu *m=ui->menuFile;
    m->actions().at(0)->setEnabled(true);
    m->actions().at(1)->setEnabled(false);
    m->actions().at(2)->setEnabled(false);
}

void MainWindow::on_actionRefresh_triggered()
{
    statusBarLabel->setText("Searching for yarprun ports");
    std::list<std::string> ports;
    theLogger->discover(ports);
    updateMain();
    theLogger->connect(ports);
    statusBarLabel->setText("Running");
}
