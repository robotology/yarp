/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "advanced_dialog.h"
#include "logtab.h"
#include "ui_logtab.h"
#include <QString>
#include <QMenu>
#include <QTextStream>
#include <ctime>
#include <yarp/logger/YarpLogger.h>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QSignalMapper>
#include <QFileDialog>
#include <QDateTime>
#include <QMimeData>

void MainWindow::updateMain()
{
    //model_yarprunports->clear();
    std::list<yarp::yarpLogger::LogEntryInfo> infos;
    this->theLogger->get_infos (infos);
    std::list<yarp::yarpLogger::LogEntryInfo>::iterator it;

    QStandardItem *itemsRoot = model_yarprunports->invisibleRootItem();
    for (it=infos.begin(); it!=infos.end(); it++)
    {
        const size_t string_size= 50;
        char time_text[string_size];
        if (it->last_update!=-1)
        {
            std::tm* tm = localtime(&it->last_update);
            sprintf(time_text, "%02d:%02d:%02d %02d/%02d/%02d", tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_mday, tm->tm_mon, tm->tm_year+1900);
        } else {
            sprintf(time_text, "no data received yet");
        }

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
                yarp::yarpLogger::LogLevel last_error = it->getLastError();

                bool     log_enabled =  this->theLogger->get_log_enable_by_port_complete(it->port_complete);
                if (log_enabled)
                {
                    if (last_error == yarp::yarpLogger::LOGLEVEL_ERROR) {
                        rowcolor = QColor("#FF7070");
                    } else if (last_error == yarp::yarpLogger::LOGLEVEL_WARNING) {
                        rowcolor = QColor("#FFFF70");
                    }
                    for (int j = 0; j < model_yarprunports->columnCount(); j++) {
                        model_yarprunports->item(i, j)->setBackground(rowcolor);
                    }
                }

                existing = true;
                if (it->last_update==-1 && !ui->actionShow_Mute_Ports->isChecked()) {model_yarprunports->removeRow(i);}
                break;
            }
        }
        if (existing == false)
        {
            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(it->ip_address.c_str())
                     << new QStandardItem(it->port_complete.c_str())
                     << new QStandardItem(time_text)
                     << new QStandardItem(logsize_text)
                     << new QStandardItem(logerrors_text)
                     << new QStandardItem (logwarnings_text);
            if (ui->actionShow_Mute_Ports->isChecked() || it->last_update!=-1)
            {
                itemsRoot->appendRow(rowItems);
            }
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
                    rowItems << new QStandardItem(it->port_prefix.c_str())
                             << new QStandardItem(it->port_complete.c_str())
                             << new QStandardItem(time_text);
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
}

void MainWindow::on_enableLogTab(int model_row)
{
    std::string logname = model_yarprunports->item(model_row,1)->text().toStdString();
    bool log_enabled = theLogger->get_log_enable_by_port_complete(logname);

    if (log_enabled)
    {
        theLogger->set_log_enable_by_port_complete(logname,false);
        for (int j = 0; j < model_yarprunports->columnCount(); j++) {
            model_yarprunports->item(model_row, j)->setBackground(QColor("#808080"));
        }
        QString message = logname.c_str() + QString(" log disabled");
        system_message->addMessage(message);
    }
    else
    {
        theLogger->set_log_enable_by_port_complete(logname,true);
        for (int j = 0; j < model_yarprunports->columnCount(); j++) {
            model_yarprunports->item(model_row, j)->setBackground(QColor(Qt::white));
        }
        QString message = logname.c_str() + QString(" log enabled");
        system_message->addMessage(message);
    }
}

void MainWindow::on_enableLogTab_action()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("on_clearLogTab_action error", MESSAGE_LEVEL_ERROR);
        return;
    }
    on_enableLogTab(model_row);
}

void MainWindow::on_clearLogTab(int model_row)
{
    QString logname = model_yarprunports->item(model_row,1)->text();
    theLogger->clear_messages_by_port_complete(logname.toStdString());
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        if (ui->logtabs->tabText(i) == logname)
        {
            auto* l = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
            if (l) {
                l->clearLogModel();
            }
            break;
        }
    }
    QString message = logname + QString(" log cleared");
    system_message->addMessage(message);
}

void MainWindow::on_clearLogTab_action()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("on_clearLogTab_action error",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_clearLogTab(model_row);
}

void MainWindow::on_resetCountersLogTab(int model_row)
{
    std::string logname = model_yarprunports->item(model_row,1)->text().toStdString();
    system_message->addMessage(QString("Counters reset for log ") + QString(logname.c_str()));
}

void MainWindow::on_saveLogTab(int model_row)
{
    std::string logname = model_yarprunports->item(model_row,1)->text().toStdString();
    std::string preferred_filename=logname;
    std::replace(preferred_filename.begin(), preferred_filename.end(), '/', '_');
    preferred_filename.erase(0,1);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export log to text file"),preferred_filename.c_str(), tr("Text Files (*.txt)"));
    if (fileName.size()!=0)
    {
        if (theLogger->export_log_to_text_file(fileName.toStdString(), logname)) {
            system_message->addMessage(QString("Current log successfully exported to file: ") + fileName);
        }
    }
}

void MainWindow::on_saveLogTab_action()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("on_saveLogTab_action error",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_saveLogTab(model_row);
}

void MainWindow::on_resetCountersLogTab_action()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("on_resetCountersLogTab_action error",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_resetCountersLogTab(model_row);
}

void MainWindow::ctxMenu(const QPoint &pos)
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("ctxMenu error",MESSAGE_LEVEL_ERROR);
        return;
    }
    std::string logname = model_yarprunports->item(model_row,1)->text().toStdString();
    bool log_enabled = theLogger->get_log_enable_by_port_complete(logname);

    auto* menu = new QMenu;
    menu->addAction(tr("Clear current log"), this, SLOT(on_clearLogTab_action()));
    menu->addAction(tr("Export current log to text file"), this, SLOT(on_saveLogTab_action()));
    menu->addAction(log_enabled ? tr("Disable current log") : tr("Enable current log"), this, SLOT(on_enableLogTab_action()));
    menu->addAction(tr("Reset errors/warning counters"), this, SLOT(on_resetCountersLogTab_action()));
    menu->exec(ui->yarprunTreeView->mapToGlobal(pos));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    yarp::os::ResourceFinder &rf = yarp::os::ResourceFinder::getResourceFinderSingleton();

    std::string loggername = rf.check("name",yarp::os::Value("/yarplogger")).asString();
    theLogger = new yarp::yarpLogger::LoggerEngine(loggername);

    ui->setupUi(this);

    system_message = ui->tab->findChild<MessageWidget*>("system_message");
    system_message->addMessage("Application Started");

    model_yarprunports = new QStandardItemModel(this);

    proxyModel = new YarprunPortsSortFilterProxyModel(this);
    proxyModel->setSourceModel(model_yarprunports);

    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(500);

    ui->yarprunTreeView->setModel(proxyModel);
    ui->yarprunTreeView->expandAll();
    selection_yarprunports=ui->yarprunTreeView->selectionModel();

    connect(ui->yarprunTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));

    bool autostart = rf.check("start");
    if (autostart)
    {
        system_message->addMessage("start option found");
        on_actionStart_Logger_triggered();
    }

    //set headers
    resetMainWindowHeaders();

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    this->theLogger->stop_logging();
    if (mainTimer) {delete mainTimer; mainTimer=nullptr;}
    if (ui)        {delete ui; ui=nullptr;}
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

void MainWindow::on_filterLineEdit_textChanged(const QString &text)
{
#if USE_FILTERS
    QString filter = "*";
    filter.append(text);
    filter.append("*");
    QRegExp regExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);

    auto* logtab = ui->logtabs->currentWidget()->findChild<LogTab*>("logtab");
    if (logtab) {
        logtab->proxyModelSearch->setFilterRegExp(regExp);
    }
#endif
}

void MainWindow::on_logtabs_tabCloseRequested(int index)
{
    delete ui->logtabs->widget(index);
}

void MainWindow::on_yarprunTreeView_doubleClicked(const QModelIndex &pre_index)
{
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("Invalid log selected",MESSAGE_LEVEL_ERROR);
        return;
    }
    QString tabname = model_yarprunports->item(model_row,1)->text();

    int exists = -1;
    for (int i = 0; i < ui->logtabs->count(); i++) {
        if (ui->logtabs->tabText(i) == tabname) {
            exists = i;
        }
    }

    if (exists>=0)
    {
        ui->logtabs->setCurrentIndex(exists);
    }
    else
    {
        auto* tab = new QTabWidget(this);
        auto* l = new QVBoxLayout(tab);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(0);
        LogTab* tmpLogTab = new LogTab(theLogger, system_message, tabname.toStdString(), this);
        tmpLogTab->displayYarprunTimestamp(ui->actionShow_YarprunTimestamps->isChecked());
        tmpLogTab->displayLocalTimestamp(ui->actionShow_LocalTimestamps->isChecked());
        tmpLogTab->displaySystemTime(ui->actionShow_System_Time->isChecked());
        tmpLogTab->displayNetworkTime(ui->actionShow_Network_Time->isChecked());
        tmpLogTab->displayExternalTime(ui->actionShow_Custom_Time->isChecked());
        tmpLogTab->displayLogLevel(ui->actionShow_Log_Level->isChecked());
        tmpLogTab->displayFilename(ui->actionShow_Filename->isChecked());
        tmpLogTab->displayLine(ui->actionShow_Line_Number->isChecked());
        tmpLogTab->displayFunction(ui->actionShow_Function->isChecked());
        tmpLogTab->displayHostname(ui->actionShow_Hostname->isChecked());
        tmpLogTab->displayPid(ui->actionShow_Pid->isChecked());
        tmpLogTab->displayCmd(ui->actionShow_Cmd->isChecked());
        tmpLogTab->displayArgs(ui->actionShow_Args->isChecked());
        tmpLogTab->displayThreadId(ui->actionShow_Thread_Id->isChecked());
        tmpLogTab->displayComponent(ui->actionShow_Component->isChecked());
        tmpLogTab->displayGrid(ui->actionShow_Grid->isChecked());
        tmpLogTab->displayColors(ui->actionShow_Colors->isChecked());
        l->addWidget(tmpLogTab);
        tmpLogTab->setObjectName("logtab");

        int newtab_index = ui->logtabs->addTab(tab, tabname);
        ui->logtabs->setCurrentIndex(newtab_index);
    }
    apply_button_filters(); //@@@@NOT WORKING HERE
}

QString MainWindow::recomputeFilters()
{
    QString filter;
    bool e_trace   = this->ui->DisplayTraceEnable->isChecked();
    bool e_debug   = this->ui->DisplayDebugEnable->isChecked();
    bool e_info    = this->ui->DisplayInfoEnable->isChecked();
    bool e_warning = this->ui->DisplayWarningEnable->isChecked();
    bool e_error   = this->ui->DisplayErrorEnable->isChecked();
    bool e_all     = this->ui->DisplayUnformattedEnable->isChecked();
    int f = 0;
    if (e_trace)                                {filter = filter + "^TRACE$";   f++;}
    if (e_debug)   {
        if (f > 0) {
            filter = filter + "|";
        }
        filter = filter + "^DEBUG$";
        f++;
    }
    if (e_info)    {
        if (f > 0) {
            filter = filter + "|";
        }
        filter = filter + "^INFO$";
        f++;
    }
    if (e_warning) {
        if (f > 0) {
            filter = filter + "|";
        }
        filter = filter + "^WARNING$";
        f++;
    }
    if (e_error)   {
        if (f > 0) {
            filter = filter + "|";
        }
        filter = filter + "^ERROR$";
        f++;
    }
    if (e_all)     {
        if (f > 0) {
            filter = filter + "|";
        }
        filter = filter + "^$";
        f++;
    }
    if (true)      {
        if (f > 0) {
            filter = filter + "|";
        }
        filter = filter + "^FATAL$";
        f++;
    }
    std::string debug = filter.toStdString();
    return filter;
}

void MainWindow::apply_button_filters()
{
#if USE_FILTERS
    QRegExp regExp ("*", Qt::CaseInsensitive, QRegExp::RegExp);
    regExp.setPattern(recomputeFilters());
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab) {
            logtab->proxyModelButtons->setFilterRegExp(regExp);
            logtab->proxyModelButtons->setFilterKeyColumn(LogModel::LOGLEVEL_COLUMN);
        }
    }
#endif
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

void MainWindow::on_DisplayTraceEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_DisplayUnformattedEnable_toggled(bool checked)
{
    apply_button_filters();
}

void MainWindow::on_actionShow_YarprunTimestamps_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayYarprunTimestamp(checked);
        }
    }
}

void MainWindow::on_actionShow_LocalTimestamps_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayLocalTimestamp(checked);
        }
    }
}

void MainWindow::on_actionShow_System_Time_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displaySystemTime(checked);
        }
    }
}

void MainWindow::on_actionShow_Network_Time_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayNetworkTime(checked);
        }
    }
}

void MainWindow::on_actionShow_Custom_Time_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayExternalTime(checked);
        }
    }
}

void MainWindow::on_actionShow_Log_Level_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayLogLevel(checked);
        }
    }
}

void MainWindow::on_actionShow_Filename_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayFilename(checked);
        }
    }
}

void MainWindow::on_actionShow_Line_Number_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayLine(checked);
        }
    }
}

void MainWindow::on_actionShow_Function_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayFunction(checked);
        }
    }
}

void MainWindow::on_actionShow_Hostname_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayHostname(checked);
        }
    }
}

void MainWindow::on_actionShow_Pid_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayPid(checked);
        }
    }
}

void MainWindow::on_actionShow_Cmd_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayCmd(checked);
        }
    }
}

void MainWindow::on_actionShow_Args_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayArgs(checked);
        }
    }
}

void MainWindow::on_actionShow_Thread_Id_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayThreadId(checked);
        }
    }
}

void MainWindow::on_actionShow_Component_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayComponent(checked);
        }
    }
}

void MainWindow::on_actionShow_Grid_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayGrid(checked);
        }
    }
}

void MainWindow::on_actionShow_Colors_toggled(bool checked)
{
    for (int i=0; i<ui->logtabs->count(); i++)
    {
        auto* logtab = ui->logtabs->widget(i)->findChild<LogTab*>("logtab");
        if (logtab)
        {
            logtab->displayColors(checked);
        }
    }
}

void MainWindow::on_actionShow_Mute_Ports_toggled(bool checked)
{
    // FIXME Do something?
}

void MainWindow::on_actionAbout_QtYarpLogger_triggered()
{
    QDesktopServices::openUrl(QUrl("http://wiki.icub.org/yarpdoc/qtyarplogger.html"));
}

void MainWindow::on_actionSave_Log_triggered(bool checked)
{
    QString dateformat = "dd_MM_yyyy_hh_mm_ss";
    QDateTime currDate = QDateTime::currentDateTime();
    QString preferred_filename = "yarprunlog_" + currDate.toString(dateformat) + ".log";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to log file"),preferred_filename, tr("Log Files (*.log)"));
    if (fileName.size()!=0)
    {
        if (theLogger->save_all_logs_to_file(fileName.toStdString())) {
            system_message->addMessage(QString("Log saved to file: ") + fileName);
        } else {
            system_message->addMessage(QString("Unable to save file: ") + fileName, MESSAGE_LEVEL_ERROR);
        }
    }
}

void MainWindow::on_actionLoad_Log_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load log file"),"./", tr("Log Files (*.log)"));
    if (fileName.size()!=0)
    {
        on_actionStop_Logger_triggered();
        on_actionClear_triggered();
        if (theLogger->load_all_logs_from_file(fileName.toStdString())) {
            system_message->addMessage(QString("Log loaded from file: ") + fileName);
        } else {
            system_message->addMessage(QString("Unable to load file: ") + fileName, MESSAGE_LEVEL_ERROR);
        }
    }
}

void MainWindow::on_actionAdvanced_triggered()
{
    QDialog* advanced = new advanced_dialog(theLogger, this);
    advanced->show();
}

void MainWindow::on_actionStart_Logger_triggered()
{
    if (this->theLogger->start_logging())
    {
        ui->actionStart_Logger->setEnabled(false);
        ui->actionStop_Logger->setEnabled(true);
        ui->actionRefresh->setEnabled(true);
        system_message->addMessage("Logger started");
    }
    else
    {
        system_message->addMessage("Unable to start: maybe logger port is conflicting with another running process?\nOnly one logger can be executed on the same network.", MESSAGE_LEVEL_ERROR);
    }
}

void MainWindow::on_actionStop_Logger_triggered()
{
    if (this->theLogger->stop_logging())
    {
        ui->actionStart_Logger->setEnabled(true);
        ui->actionStop_Logger->setEnabled(false);
        ui->actionRefresh->setEnabled(false);
        system_message->addMessage("Logger stopped");
    }
    else
    {
        system_message->addMessage("Unable to stop: logger is not currently running",MESSAGE_LEVEL_ERROR);
    }
}

void MainWindow::on_actionRefresh_triggered()
{
    system_message->addMessage("Searching for yarprun ports");
    std::list<std::string> ports;
    theLogger->discover(ports);
    updateMain();
    theLogger->connect(ports);
    char text [100];
    sprintf (text,"found %zd ports, logger running", ports.size());
    system_message->addMessage(text);
}

void MainWindow::resetMainWindowHeaders()
{
    model_yarprunports->setHorizontalHeaderItem(0, new QStandardItem("ip"));
    model_yarprunports->setHorizontalHeaderItem(1, new QStandardItem("process"));
    model_yarprunports->setHorizontalHeaderItem(2, new QStandardItem("last heard"));
    model_yarprunports->setHorizontalHeaderItem(3, new QStandardItem("log size"));
    model_yarprunports->setHorizontalHeaderItem(4, new QStandardItem("errors"));
    model_yarprunports->setHorizontalHeaderItem(5, new QStandardItem("warnings"));
    ui->yarprunTreeView->setColumnWidth(0, 100);
    ui->yarprunTreeView->setColumnWidth(1, 200);
    ui->yarprunTreeView->setColumnWidth(2, 150);
    ui->yarprunTreeView->setColumnWidth(3, 80);
    ui->yarprunTreeView->setColumnWidth(4, 60);
    ui->yarprunTreeView->setColumnWidth(5, 60);
}

void MainWindow::on_actionClear_triggered()
{
    if (theLogger->clear())
    {
        if (model_yarprunports)
        {
            model_yarprunports->clear();
            resetMainWindowHeaders();
        }
        if (ui->logtabs) {
            ui->logtabs->clear();
        }

        system_message->addMessage("Log cleared");
    }
}

void MainWindow::on_actionClear_current_log_triggered()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("Invalid log selected",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_clearLogTab(model_row);
}

void MainWindow::on_actionExport_current_log_to_text_file_triggered()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("Invalid log selected",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_saveLogTab(model_row);
}

void MainWindow::on_actionDisable_current_log_triggered()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("Invalid log selected",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_enableLogTab(model_row);
}

void MainWindow::on_actionReset_current_log_error_warning_counters_triggered()
{
    QModelIndex pre_index = ui->yarprunTreeView->selectionModel()->currentIndex();
    QModelIndex index = proxyModel->mapToSource(pre_index);
    int model_row=index.row();
    if (model_row==-1)
    {
        system_message->addMessage("Invalid log selected",MESSAGE_LEVEL_ERROR);
        return;
    }
    on_resetCountersLogTab(model_row);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    const QUrl url = e->mimeData()->urls().first();

    QString fileName = url.toLocalFile();

    on_actionStop_Logger_triggered();
    on_actionClear_triggered();

    if (theLogger->load_all_logs_from_file(fileName.toStdString())) {
        system_message->addMessage(QString("Log loaded from file: ") + fileName);
    } else {
        system_message->addMessage(QString("Unable to load file: ") + fileName, MESSAGE_LEVEL_ERROR);
    }
}
