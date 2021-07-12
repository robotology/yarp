/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "logmodel.h"

#include <QBrush>
#include <QFontDatabase>

const QString TRACE_STRING   = "TRACE";
const QString DEBUG_STRING   = "DEBUG";
const QString INFO_STRING    = "INFO";
const QString WARNING_STRING = "WARNING";
const QString ERROR_STRING   = "ERROR";
const QString FATAL_STRING   = "FATAL";


LogModel::LogModel(QObject *parent) :
        QAbstractTableModel(parent),
        m_font(QFontDatabase::systemFont(QFontDatabase::FixedFont)),
        m_names(QAbstractTableModel::roleNames())
{
    m_names[YarprunTimestampRole] = "yarprun_timestamp";
    m_names[LocalTimestampRole] = "local_timestamp";
    m_names[SystemTimeRole] = "systemtime";
    m_names[SystemTimeStringRole] = "systemtime_string";
    m_names[NetworkTimeRole] = "networktime";
    m_names[NetworkTimeStringRole] = "networktime_string";
    m_names[ExternalTimeRole] = "externaltime";
    m_names[ExternalTimeStringRole] = "externaltime_string";
    m_names[LogLevelRole] = "level";
    m_names[LogLevelStringRole] = "level_string";
    m_names[FilenameRole] = "filename";
    m_names[LineRole] = "line";
    m_names[LineStringRole] = "line_string";
    m_names[FunctionRole] = "function";
    m_names[HostnameRole] = "hostname";
    m_names[CmdRole] = "cmd";
    m_names[ArgsRole] = "args";
    m_names[PidRole] = "pid";
    m_names[ThreadIdRole] = "thread_id";
    m_names[ThreadIdStringRole] = "thread_id_string";
    m_names[ComponentRole] = "component";
    m_names[TextRole] = "text";
    m_names[BacktraceRole] = "backtrace";
}

LogModel::~LogModel() = default;

int LogModel::rowCount(const QModelIndex &parent) const
{
    // If the index is the root item, then return the row count,
    // otherwise return 0
    if (parent == QModelIndex()) {
        return m_messages.size();
    }
    return 0;
}

int LogModel::columnCount(const QModelIndex &parent) const
{
    // If the index is the root item, then return the column count,
    // otherwise return 0
    if (parent == QModelIndex()) {
        return COLUMN_COUNT;
    }
    return 0;
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case YARPRUNTIMESTAMP_COLUMN:
        return tr("yarprun timestamp");
    case LOCALTIMESTAMP_COLUMN:
        return tr("local timestamp");
    case SYSTEMTIME_COLUMN:
        return tr("system time");
    case NETWORKTIME_COLUMN:
        return tr("network time");
    case EXTERNALTIME_COLUMN:
        return tr("external time");
    case LOGLEVEL_COLUMN:
        return tr("level");
    case FILENAME_COLUMN:
        return tr("filename");
    case LINE_COLUMN:
        return tr("line");
    case FUNCTION_COLUMN:
        return tr("function");
    case HOSTNAME_COLUMN:
        return tr("hostname");
    case CMD_COLUMN:
        return tr("cmd");
    case ARGS_COLUMN:
        return tr("args");
    case PID_COLUMN:
        return tr("pid");
    case THREADID_COLUMN:
        return tr("thread id");
    case COMPONENT_COLUMN:
        return tr("component");
    case TEXT_COLUMN:
        return tr("message");
    default:
        return QVariant();
    }
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case YARPRUNTIMESTAMP_COLUMN:
            return data(index, YarprunTimestampRole);
        case LOCALTIMESTAMP_COLUMN:
            return data(index, LocalTimestampRole);
        case SYSTEMTIME_COLUMN:
            return data(index, SystemTimeStringRole);
        case NETWORKTIME_COLUMN:
            return data(index, NetworkTimeStringRole);
        case EXTERNALTIME_COLUMN:
            return data(index, ExternalTimeStringRole);
        case LOGLEVEL_COLUMN:
            return data(index, LogLevelStringRole);
        case FILENAME_COLUMN:
            return data(index, FilenameRole);
        case LINE_COLUMN:
            return data(index, LineStringRole);
        case FUNCTION_COLUMN:
            return data(index, FunctionRole);
        case CMD_COLUMN:
            return data(index, CmdRole);
        case HOSTNAME_COLUMN:
            return data(index, HostnameRole);
        case ARGS_COLUMN:
            return data(index, ArgsRole);
        case PID_COLUMN:
            return data(index, PidStringRole);
        case THREADID_COLUMN:
            return data(index, ThreadIdStringRole);
        case COMPONENT_COLUMN:
            return data(index, ComponentRole);
        case TEXT_COLUMN:
            return data(index, TextRole);
        default:
            return QString();
        }
    }

    if (role == Qt::BackgroundRole) {
        if(!m_color) {
            return QBrush();
        }
        switch (static_cast<yarp::yarpLogger::LogLevelEnum>(m_messages.at(index.row()).level)) {
        case yarp::yarpLogger::LOGLEVEL_UNDEFINED:
            return QBrush(Qt::white);
        case yarp::yarpLogger::LOGLEVEL_TRACE:
            return QBrush(QColor("#E9E9E9"));
        case yarp::yarpLogger::LOGLEVEL_DEBUG:
            return QBrush(QColor("#78E678"));
        case yarp::yarpLogger::LOGLEVEL_INFO:
            return QBrush(QColor("#8DCFE8"));
        case yarp::yarpLogger::LOGLEVEL_WARNING:
            return QBrush(QColor("#FFFF70"));
        case yarp::yarpLogger::LOGLEVEL_ERROR:
            return QBrush(QColor("#DE4E4E"));
        case yarp::yarpLogger::LOGLEVEL_FATAL:
            return QBrush(Qt::black);
        default:
            return QBrush(Qt::white);
        }
    }

    if (role == Qt::ForegroundRole) {
        if(!m_color) {
            return QBrush();
        }
        if (m_messages.at(index.row()).level == yarp::yarpLogger::LOGLEVEL_FATAL) {
            return QBrush(Qt::white);
        }
        return QBrush(Qt::black);
    }

    if (role == Qt::FontRole) {
        return m_font;
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case LOCALTIMESTAMP_COLUMN:
        case SYSTEMTIME_COLUMN:
        case NETWORKTIME_COLUMN:
        case EXTERNALTIME_COLUMN:
        case LINE_COLUMN:
        case PID_COLUMN:
        case THREADID_COLUMN:
            return QVariant(Qt::AlignRight | (m_wordwrap ? Qt::AlignTop :  Qt::AlignVCenter));
        case YARPRUNTIMESTAMP_COLUMN:
        case LOGLEVEL_COLUMN:
        case FILENAME_COLUMN:
        case FUNCTION_COLUMN:
        case HOSTNAME_COLUMN:
        case CMD_COLUMN:
        case ARGS_COLUMN:
        case COMPONENT_COLUMN:
        case TEXT_COLUMN:
        default:
            return QVariant(Qt::AlignLeft | (m_wordwrap ? Qt::AlignTop :  Qt::AlignVCenter));
        }
    }

    if (role == Qt::ToolTipRole) {
        return data(index, BacktraceRole);
    }

    if (role == YarprunTimestampRole) {
        return QString(m_messages.at(index.row()).yarprun_timestamp.c_str());
    }

    if (role == LocalTimestampRole) {
        return QString(m_messages.at(index.row()).local_timestamp.c_str());
    }

    if (role == SystemTimeRole) {
        return m_messages.at(index.row()).systemtime;
    }

    if (role == SystemTimeStringRole) {
        auto systemtime = m_messages.at(index.row()).systemtime;
        if (systemtime != 0.0) {
            return QString::number(systemtime, 'f'); // FIXME Return properly formatted date/time
        }
        return QVariant();
    }

    if (role == NetworkTimeRole) {
        return m_messages.at(index.row()).networktime;
    }

    if (role == NetworkTimeStringRole) {
        auto networktime = m_messages.at(index.row()).networktime;
        if (networktime != 0.0) {
            return QString::number(networktime, 'f'); // FIXME Return properly formatted date/time
        }
        return QVariant();
    }

    if (role == ExternalTimeRole) {
        return m_messages.at(index.row()).externaltime;
    }

    if (role == ExternalTimeStringRole) {
        auto externaltime = m_messages.at(index.row()).externaltime;
        if (externaltime != 0.0) {
            return QString::number(externaltime, 'f'); // FIXME Return properly formatted date/time
        }
        return QVariant();
    }

    if (role == LogLevelRole) {
        return static_cast<yarp::yarpLogger::LogLevelEnum>(m_messages.at(index.row()).level);
    }

    if (role == LogLevelStringRole) {
        return logLevelToString(m_messages.at(index.row()).level);
    }

    if (role == FilenameRole) {
        return QString(m_messages.at(index.row()).filename.c_str());
    }

    if (role == LineRole) {
        return m_messages.at(index.row()).line;
    }

    if (role == LineStringRole) {
        auto line = m_messages.at(index.row()).line;
        if (line != 0) {
            return QString::number(line);
        }
    }

    if (role == FunctionRole) {
        return QString(m_messages.at(index.row()).function.c_str());
    }

    if (role == HostnameRole) {
        return QString(m_messages.at(index.row()).hostname.c_str());
    }

    if (role == CmdRole) {
        return QString(m_messages.at(index.row()).cmd.c_str());
    }

    if (role == ArgsRole) {
        return QString(m_messages.at(index.row()).args.c_str());
    }

    if (role == PidRole) {
        return m_messages.at(index.row()).pid;
    }

    if (role == PidStringRole) {
        auto pid = m_messages.at(index.row()).pid;
        if (pid != 0) {
            return QString::number(pid);
        }
        return QVariant();
    }

    if (role == ThreadIdRole) {
        return static_cast<qlonglong>(m_messages.at(index.row()).thread_id);
    }

    if (role == ThreadIdStringRole) {
        auto thread_id = m_messages.at(index.row()).thread_id;
        if (thread_id != 0) {
            return QString("0x") + QString::number(thread_id, 16).rightJustified(8, '0');
        }
        return QVariant();
    }

    if (role == ComponentRole) {
        return QString(m_messages.at(index.row()).component.c_str());
    }

    if (role == TextRole) {
        return QString(m_messages.at(index.row()).text.c_str());
    }

    if (role == BacktraceRole) {
        return QString(m_messages.at(index.row()).backtrace.c_str());
    }

    return QVariant();
}

QHash<int, QByteArray> LogModel::roleNames() const
{
    return m_names;
}

void LogModel::addMessages(const std::list<yarp::yarpLogger::MessageEntry> &m_messages)
{
    beginInsertRows(QModelIndex(),
                    rowCount(),
                    rowCount() + m_messages.size() - 1);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    this->m_messages.append(QList<yarp::yarpLogger::MessageEntry>(m_messages.begin(), m_messages.end()));
#else
    this->m_messages.append(QList<yarp::yarpLogger::MessageEntry>::fromStdList(m_messages));
#endif
    endInsertRows();
}

void LogModel::setColor(bool enabled)
{
    if (m_color != enabled) {
        m_color = enabled;
        Q_EMIT dataChanged(index(0, 0),
                           index(rowCount()-1,columnCount()-1),
                           QVector<int>() << Qt::BackgroundRole
                                          << Qt::ForegroundRole);
    }
}

void LogModel::setWordWrap(bool wordwrap)
{
    if (m_wordwrap != wordwrap) {
        m_wordwrap = wordwrap;
        Q_EMIT dataChanged(index(0, 0),
                           index(rowCount()-1,columnCount()-1),
                           QVector<int>() << Qt::TextAlignmentRole);
    }
}

void LogModel::clear()
{
    beginResetModel();
    this->m_messages.clear();
    endResetModel();
}

QString LogModel::logLevelToString(yarp::yarpLogger::LogLevel l)
{
    switch (static_cast<yarp::yarpLogger::LogLevelEnum>(l)) {
    case yarp::yarpLogger::LOGLEVEL_TRACE:
        return TRACE_STRING;
    case yarp::yarpLogger::LOGLEVEL_DEBUG:
        return DEBUG_STRING;
    case yarp::yarpLogger::LOGLEVEL_INFO:
        return INFO_STRING;
    case yarp::yarpLogger::LOGLEVEL_WARNING:
        return WARNING_STRING;
    case yarp::yarpLogger::LOGLEVEL_ERROR:
        return ERROR_STRING;
    case yarp::yarpLogger::LOGLEVEL_FATAL:
        return FATAL_STRING;
    case yarp::yarpLogger::LOGLEVEL_UNDEFINED:
    default:
        return "";
    }
}
