/* 
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include "logtab.h"
#include "ui_logtab.h"

#include <QFontDatabase>
#include <QSortFilterProxyModel>

LogTab::LogTab(yarp::yarpLogger::LoggerEngine*  _theLogger, MessageWidget* _system_message, std::string _portName, QWidget *parent, int refreshRate) :
    QFrame(parent),
    ui(new Ui::LogTab),
    portName(_portName),
    theLogger(_theLogger),
    system_message(_system_message),
    displayYarprunTimestamp_enabled(true),
    displayLocalTimestamp_enabled(true),
    displayErrorLevel_enabled(true),
    displayColors_enabled(true),
    displayGrid_enabled(true),
    toggleLineExpansion(false),
    clipboard(QApplication::clipboard()),
    logViewModel(new LoggerViewModel(*_theLogger, portName)),
    logLevelFilterModelProxy(new QSortFilterProxyModel(this)),
    messageFilterModelProxy(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);

    // This hierachy of proxies can be substituted with a single
    // proxy. In this case, we have to inherit and implement
    // filterAcceptRows and call the invalidate() function when needed
    logLevelFilterModelProxy->setSourceModel(logViewModel);
    logLevelFilterModelProxy->setFilterKeyColumn(LoggerViewModel::LogLevlColumnIndex); // level column

    messageFilterModelProxy->setSourceModel(logLevelFilterModelProxy);
    messageFilterModelProxy->setFilterKeyColumn(LoggerViewModel::MessageColumnIndex); // message column
    messageFilterModelProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->listView->setModel(messageFilterModelProxy);

    ui->listView->setSortingEnabled(true);
    ui->listView->sortByColumn(LoggerViewModel::YarpRunTimeColumnIndex, Qt::AscendingOrder);


    ui->listView->verticalHeader()->setVisible(false);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listView->setAutoScroll(true);

    //???: understand if the view is smart enough to avoid too many calls
    // otherwise use timer??


    ui->listView->setColumnWidth(0,120);
    ui->listView->setColumnWidth(LoggerViewModel::YarpRunTimeColumnIndex, 120);
    ui->listView->setColumnWidth(LoggerViewModel::LocalTimeColumnIndex, 120);
    ui->listView->setColumnWidth(LoggerViewModel::LogLevlColumnIndex, 120);
    ui->listView->setColumnWidth(LoggerViewModel::MessageColumnIndex, 100);
    ui->listView->horizontalHeader()->setSectionResizeMode(LoggerViewModel::MessageColumnIndex, QHeaderView::Stretch);
    ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->listView->verticalHeader()->setDefaultSectionSize(20);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->listView->setFont(fixedFont);

    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));
    connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(expandLines()));
}


void LogTab::expandLines() {
    toggleLineExpansion = !toggleLineExpansion;
    if ( toggleLineExpansion )
        ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    else {
        ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->listView->verticalHeader()->setDefaultSectionSize(20);
    }
}

void LogTab::ctxMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu;
    menu->addAction(tr("Copy to clipboard"), this, SLOT(on_copy_to_clipboard_action()));
    menu->addAction(tr("Toggle line expansion"), this, SLOT(expandLines()));
    menu->exec(ui->listView->mapToGlobal(pos));
}

void LogTab::on_copy_to_clipboard_action()
{
    //FIXME: mapToSource changes indices even if it should not
    QString selected_test;
    QString separator("\t");
    foreach(const QModelIndex &index, ui->listView->selectionModel()->selectedRows())
    {
        QStringList list;
        QModelIndex prox_index_pre = messageFilterModelProxy->mapToSource(index);
        if (prox_index_pre.isValid() == false)
        {
            system_message->addMessage(QString("Invalid prox_index_pre in copy_to_clipboard"));
            return;
        }
        QModelIndex prox_index     = logLevelFilterModelProxy->mapToSource(prox_index_pre);
        if (prox_index.isValid() == false)
        {
            system_message->addMessage(QString("Invalid prox_index in copy_to_clipboard"));
            return;
        }
        if (displayYarprunTimestamp_enabled) list.append(logViewModel->data(logViewModel->index(prox_index.row(), LoggerViewModel::YarpRunTimeColumnIndex)).toString());
        if (displayLocalTimestamp_enabled)   list.append(logViewModel->data(logViewModel->index(prox_index.row(), LoggerViewModel::LocalTimeColumnIndex)).toString());
        if (displayErrorLevel_enabled)       list.append(logViewModel->data(logViewModel->index(prox_index.row(), LoggerViewModel::LogLevlColumnIndex)).toString());
        list.append(logViewModel->data(logViewModel->index(prox_index.row(), LoggerViewModel::MessageColumnIndex)).toString());
        selected_test += list.join(separator);
        selected_test += '\n';
    }
    clipboard->setText(selected_test);
}

LogTab::~LogTab()
{
    delete messageFilterModelProxy;
    delete logLevelFilterModelProxy;
    delete ui;
    delete logViewModel;
}

void LogTab::clear_model_logs()
{
    mutex.lock();
    if (logViewModel) logViewModel->removeRows(0, logViewModel->rowCount());
    mutex.unlock();
}

void LogTab::displayYarprunTimestamp  (bool enabled)
{
    displayYarprunTimestamp_enabled=enabled;
    ui->listView->setColumnHidden(LoggerViewModel::YarpRunTimeColumnIndex,!displayYarprunTimestamp_enabled);
}

void LogTab::displayLocalTimestamp  (bool enabled)
{
    displayLocalTimestamp_enabled=enabled;
    ui->listView->setColumnHidden(LoggerViewModel::LocalTimeColumnIndex,!displayLocalTimestamp_enabled);
}

void LogTab::displayErrorLevel(bool enabled)
{
    displayErrorLevel_enabled=enabled;
    ui->listView->setColumnHidden(LoggerViewModel::LogLevlColumnIndex,!displayErrorLevel_enabled);
}

void LogTab::displayColors(bool enabled)
{
    displayColors_enabled=enabled;
}

void LogTab::displayGrid(bool enabled)
{
    displayGrid_enabled=enabled;
    ui->listView->setShowGrid(displayGrid_enabled);
}

void LogTab::filterByLevel(QString levelString)
{
    QRegExp regExp(levelString, Qt::CaseInsensitive, QRegExp::RegExp);
    logLevelFilterModelProxy->setFilterRegExp(regExp);
}

void LogTab::filterByMessage(QString message)
{
    QString filter = "*";
    filter.append(message);
    filter.append("*");
    QRegExp regExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
    messageFilterModelProxy->setFilterRegExp(regExp);
}
