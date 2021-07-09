/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "logtab.h"
#include "ui_logtab.h"

#include <utility>
#include <QFontMetrics>

LogTab::LogTab(yarp::yarpLogger::LoggerEngine* _theLogger,
               MessageWidget* _system_message,
               std::string _portName,
               QWidget *parent,
               int refreshRate) :
        QFrame(parent),
        ui(new Ui::LogTab),
        portName(std::move(_portName)),
        theLogger(_theLogger),
        system_message(_system_message),
        displayYarprunTimestamp_enabled(true),
        displayLocalTimestamp_enabled(true),
        displaySystemTime_enabled(false),
        displayNetworkTime_enabled(false),
        displayExternalTime_enabled(false),
        displayLogLevel_enabled(true),
        displayFilename_enabled(false),
        displayLine_enabled(false),
        displayFunction_enabled(false),
        displayHostname_enabled(false),
        displayPid_enabled(false),
        displayCmd_enabled(false),
        displayArgs_enabled(false),
        displayThreadId_enabled(false),
        displayComponent_enabled(true),
        displayColors_enabled(true),
        displayGrid_enabled(true),
        toggleLineExpansion(false),
        logTimer(new QTimer(this)),
        logModel(new LogModel()),
#if USE_FILTERS
        proxyModelButtons(new LogSortFilterProxyModel(this)),
        proxyModelSearch(new LogSortFilterProxyModel(this)),
#endif
        clipboard(QApplication::clipboard())
{
    ui->setupUi(this);

#if USE_FILTERS
    proxyModelButtons->setSourceModel(logModel);
    proxyModelSearch->setSourceModel(proxyModelButtons);
    proxyModelSearch->setFilterKeyColumn(-1);
    ui->listView->setModel(proxyModelSearch);
#else
    ui->listView->setModel(logModel);
#endif

    connect(logTimer, SIGNAL(timeout()), this, SLOT(updateLog()));
    logTimer->start(refreshRate);

    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ctxMenu(const QPoint &)));
    connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(expandLines()));

    ui->listView->horizontalHeader()->setSectionsMovable(true);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::YARPRUNTIMESTAMP_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::LOCALTIMESTAMP_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::SYSTEMTIME_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::NETWORKTIME_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::EXTERNALTIME_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::LOGLEVEL_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::LINE_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::HOSTNAME_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::PID_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::CMD_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::THREADID_COLUMN, QHeaderView::ResizeToContents);
    ui->listView->horizontalHeader()->setSectionResizeMode(LogModel::COMPONENT_COLUMN, QHeaderView::ResizeToContents);

    ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    sectionHeight = QFontMetrics(qvariant_cast<QFont>(logModel->data(QModelIndex(), Qt::FontRole))).height() + 4;
    ui->listView->verticalHeader()->setMinimumSectionSize(sectionHeight);
    ui->listView->verticalHeader()->setDefaultSectionSize(sectionHeight);

    updateLog(true);
}


void LogTab::expandLines() {
    toggleLineExpansion = !toggleLineExpansion;
    ui->listView->setWordWrap(toggleLineExpansion);
    logModel->setWordWrap(toggleLineExpansion);

    if (toggleLineExpansion) {
        ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else {
        ui->listView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->listView->verticalHeader()->setMinimumSectionSize(sectionHeight);
        ui->listView->verticalHeader()->setDefaultSectionSize(sectionHeight);
    }
}

void LogTab::ctxMenu(const QPoint &pos)
{
    auto* menu = new QMenu;
    menu->addAction(tr("Copy to clipboard"), this, SLOT(on_copy_to_clipboard_action()));
    menu->addAction(tr("Toggle line expansion"), this, SLOT(expandLines()));
    menu->exec(ui->listView->mapToGlobal(pos));
}

void LogTab::on_copy_to_clipboard_action()
{
    QString selected_test;
    QString separator("\t");
    foreach(const QModelIndex &index, ui->listView->selectionModel()->selectedRows())
    {
        QStringList list;
#if USE_FILTERS
        QModelIndex prox_index_pre = proxyModelSearch->mapToSource(index);
        if (!prox_index_pre.isValid())
        {
            system_message->addMessage(QString("Invalid prox_index_pre in copy_to_clipboard"));
            return;
        }
        QModelIndex prox_index     = proxyModelButtons->mapToSource(prox_index_pre);
        if (!prox_index.isValid())
        {
            system_message->addMessage(QString("Invalid prox_index in copy_to_clipboard"));
            return;
        }
#else
        QModelIndex prox_index = index;
#endif
        if (displayYarprunTimestamp_enabled) { list.append(logModel->data(prox_index, LogModel::YarprunTimestampRole).toString()); }
        if (displayLocalTimestamp_enabled)   { list.append(logModel->data(prox_index, LogModel::LocalTimestampRole).toString()); }
        if (displaySystemTime_enabled)       { list.append(logModel->data(prox_index, LogModel::SystemTimeStringRole).toString()); }
        if (displayNetworkTime_enabled)      { list.append(logModel->data(prox_index, LogModel::NetworkTimeStringRole).toString()); }
        if (displayExternalTime_enabled)     { list.append(logModel->data(prox_index, LogModel::ExternalTimeStringRole).toString()); }
        if (displayLogLevel_enabled)         { list.append(logModel->data(prox_index, LogModel::LogLevelStringRole).toString()); }
        if (displayFilename_enabled)         { list.append(logModel->data(prox_index, LogModel::FilenameRole).toString()); }
        if (displayLine_enabled)             { list.append(logModel->data(prox_index, LogModel::LineStringRole).toString()); }
        if (displayFunction_enabled)         { list.append(logModel->data(prox_index, LogModel::FunctionRole).toString()); }
        if (displayHostname_enabled)         { list.append(logModel->data(prox_index, LogModel::HostnameRole).toString()); }
        if (displayPid_enabled)              { list.append(logModel->data(prox_index, LogModel::PidStringRole).toString()); }
        if (displayCmd_enabled)              { list.append(logModel->data(prox_index, LogModel::CmdRole).toString()); }
        if (displayArgs_enabled)             { list.append(logModel->data(prox_index, LogModel::ArgsRole).toString()); }
        if (displayThreadId_enabled)         { list.append(logModel->data(prox_index, LogModel::ThreadIdRole).toString()); }
        if (displayComponent_enabled)        { list.append(logModel->data(prox_index, LogModel::ComponentRole).toString()); }
        list.append(logModel->data(prox_index, LogModel::TextRole).toString());
        selected_test += list.join(separator);
        selected_test += '\n';
    }
    clipboard->setText(selected_test);
}

LogTab::~LogTab()
{
    delete logTimer;
    delete logModel;
#if USE_FILTERS
    delete proxyModelButtons;
    delete proxyModelSearch;
#endif
    delete ui;
}

void LogTab::clearLogModel()
{
    mutex.lock();
    if (logModel) {
        logModel->clear();
    }
    mutex.unlock();
}

void LogTab::updateLog(bool from_beginning)
{
    mutex.lock();
    std::list<yarp::yarpLogger::MessageEntry> messages;
    theLogger->get_messages_by_port_complete(portName, messages, from_beginning);
    logModel->addMessages(messages);

    ui->listView->setColumnHidden(LogModel::YARPRUNTIMESTAMP_COLUMN, !displayYarprunTimestamp_enabled);
    ui->listView->setColumnHidden(LogModel::LOCALTIMESTAMP_COLUMN,   !displayLocalTimestamp_enabled);
    ui->listView->setColumnHidden(LogModel::SYSTEMTIME_COLUMN,       !displaySystemTime_enabled);
    ui->listView->setColumnHidden(LogModel::NETWORKTIME_COLUMN,      !displayNetworkTime_enabled);
    ui->listView->setColumnHidden(LogModel::EXTERNALTIME_COLUMN,     !displayExternalTime_enabled);
    ui->listView->setColumnHidden(LogModel::LOGLEVEL_COLUMN,         !displayLogLevel_enabled);
    ui->listView->setColumnHidden(LogModel::FILENAME_COLUMN,         !displayFilename_enabled);
    ui->listView->setColumnHidden(LogModel::LINE_COLUMN,             !displayLine_enabled);
    ui->listView->setColumnHidden(LogModel::FUNCTION_COLUMN,         !displayFunction_enabled);
    ui->listView->setColumnHidden(LogModel::HOSTNAME_COLUMN,         !displayHostname_enabled);
    ui->listView->setColumnHidden(LogModel::PID_COLUMN,              !displayPid_enabled);
    ui->listView->setColumnHidden(LogModel::CMD_COLUMN,              !displayCmd_enabled);
    ui->listView->setColumnHidden(LogModel::ARGS_COLUMN,             !displayArgs_enabled);
    ui->listView->setColumnHidden(LogModel::COMPONENT_COLUMN,        !displayComponent_enabled);
    ui->listView->setShowGrid(displayGrid_enabled);
    mutex.unlock();
}

void LogTab::displayYarprunTimestamp(bool enabled)
{
    displayYarprunTimestamp_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::YARPRUNTIMESTAMP_COLUMN, !displayYarprunTimestamp_enabled);
}

void LogTab::displayLocalTimestamp(bool enabled)
{
    displayLocalTimestamp_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::LOCALTIMESTAMP_COLUMN, !displayLocalTimestamp_enabled);
}

void LogTab::displaySystemTime(bool enabled)
{
    displaySystemTime_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::SYSTEMTIME_COLUMN, !displaySystemTime_enabled);
}

void LogTab::displayNetworkTime(bool enabled)
{
    displayNetworkTime_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::SYSTEMTIME_COLUMN, !displayNetworkTime_enabled);
}

void LogTab::displayExternalTime(bool enabled)
{
    displayExternalTime_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::SYSTEMTIME_COLUMN, !displayExternalTime_enabled);
}


void LogTab::displayLogLevel(bool enabled)
{
    displayLogLevel_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::LOGLEVEL_COLUMN, !displayLogLevel_enabled);
}

void LogTab::displayFilename(bool enabled)
{
    displayFilename_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::FILENAME_COLUMN, !displayFilename_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(LogModel::FILENAME_COLUMN, 120);
    }
}

void LogTab::displayLine(bool enabled)
{
    displayLine_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::LINE_COLUMN, !displayLine_enabled);
}

void LogTab::displayFunction(bool enabled)
{
    displayFunction_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::FUNCTION_COLUMN, !displayFunction_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(LogModel::FUNCTION_COLUMN, 120);
    }
}

void LogTab::displayHostname(bool enabled)
{
    displayHostname_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::THREADID_COLUMN, !displayHostname_enabled);
}

void LogTab::displayCmd(bool enabled)
{
    displayCmd_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::THREADID_COLUMN, !displayCmd_enabled);
}

void LogTab::displayArgs(bool enabled)
{
    displayArgs_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::THREADID_COLUMN, !displayArgs_enabled);
    if (enabled) {
        ui->listView->setColumnWidth(LogModel::FUNCTION_COLUMN, 120);
    }
}

void LogTab::displayPid(bool enabled)
{
    displayPid_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::THREADID_COLUMN, !displayPid_enabled);
}

void LogTab::displayThreadId(bool enabled)
{
    displayThreadId_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::THREADID_COLUMN, !displayThreadId_enabled);
}

void LogTab::displayComponent(bool enabled)
{
    displayComponent_enabled = enabled;
    ui->listView->setColumnHidden(LogModel::COMPONENT_COLUMN, !displayComponent_enabled);
}

void LogTab::displayColors(bool enabled)
{
    displayColors_enabled = enabled;
    logModel->setColor(enabled);
}

void LogTab::displayGrid(bool enabled)
{
    displayGrid_enabled = enabled;
    ui->listView->setShowGrid(displayGrid_enabled);
}
