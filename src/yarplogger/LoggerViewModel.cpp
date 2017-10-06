#include "LoggerViewModel.h"

#include <QSize>
#include <QColor>

const size_t LoggerViewModel::YarpRunTimeColumnIndex = 0;
const size_t LoggerViewModel::LocalTimeColumnIndex = 1;
const size_t LoggerViewModel::LogLevlColumnIndex = 2;
const size_t LoggerViewModel::MessageColumnIndex = 3;

LoggerViewModel::LoggerViewModel(yarp::yarpLogger::LoggerEngine& loggerEngine,
                                 const std::string& portName)
: m_loggerEngine(loggerEngine)
, m_portName(portName)
, m_observedEntry(nullptr)
{
    //for now mantain a full copy of the log, and update this through notifications
    //We may want to obtain a reference only to the log, but then how do we
    //fire the notifications to the GUI?
    m_loggerEngine.get_messages_by_port_complete(m_portName, m_logMessages, true);
    m_loggerEngine.getLogEntryByPortComplete(m_portName, m_observedEntry);
    m_observedEntry->addObserver(*this);
}

LoggerViewModel::~LoggerViewModel()
{
    m_observedEntry->removeObserver(*this);
}

int LoggerViewModel::rowCount(const QModelIndex &parent) const
{
    return m_observedEntry->entry_list.size();
}
int LoggerViewModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant LoggerViewModel::data(const QModelIndex &index, int role) const
{
    // check indices
    if (index.row() < 0 || static_cast<size_t>(index.row()) >= m_observedEntry->entry_list.size()) {
        return QVariant();
    }

    // check admissible role
    if (role != Qt::BackgroundRole && role != Qt::ForegroundRole
        && role != Qt::DisplayRole) {
        return QVariant();
    }

    // get the message
    yarp::yarpLogger::MessageEntry& entry = m_observedEntry->entry_list[index.row()];

    if (role == Qt::ForegroundRole) {
        if (entry.level == yarp::yarpLogger::LOGLEVEL_FATAL) {
            return QColor(Qt::white);
        }
        return QColor(Qt::black);
    }

    if (role == Qt::BackgroundRole) {
        switch (entry.level) {
            case yarp::yarpLogger::LOGLEVEL_TRACE:
                return QColor("#FF70FF");
            case yarp::yarpLogger::LOGLEVEL_DEBUG:
                return QColor("#7070FF");
            case yarp::yarpLogger::LOGLEVEL_INFO:
                return QColor("#70FF70");
            case yarp::yarpLogger::LOGLEVEL_WARNING:
                return QColor("#FFFF70");
            case yarp::yarpLogger::LOGLEVEL_ERROR:
                return QColor("#FF7070");
            case yarp::yarpLogger::LOGLEVEL_FATAL:
                return QColor(Qt::black);
            case yarp::yarpLogger::LOGLEVEL_UNDEFINED:
            default:
                return QColor(Qt::white);
        }
    }

    if (role == Qt::DisplayRole) {

        QVariant data;
        switch (index.column()) {
//            case 0:
//                // human time
//            {
//                QString str(entry.local_timestamp.c_str());
//                data = QVariant(str.toDouble());
//                break;
//            }
            case YarpRunTimeColumnIndex:
                //yarp run timestamp
            {
                QString str(entry.yarprun_timestamp.c_str());
                data = QVariant(str.toDouble());
                break;
            }
            case LocalTimeColumnIndex:
                // local timestamp
                data = QVariant(entry.local_timestamp.c_str());
                break;
            case LogLevlColumnIndex:
                // Level
            {
                std::string error_level = entry.level;
                data = QVariant(error_level.c_str());
                break;
            }
            case MessageColumnIndex:
                // message
            {
                QString message(entry.text.c_str());
                data = QVariant(message.trimmed());
                break;
            }

            default:
                break;
        }
        return data;
    }
    return QVariant();
}

QVariant LoggerViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        // Invalid mode
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        switch (section) {
//            case 0:
//                return QVariant(QString("Time"));
            case YarpRunTimeColumnIndex:
                return QVariant(QString("yarprun timestamp"));
            case LocalTimeColumnIndex:
                return QVariant(QString("local timestamp"));
            case LogLevlColumnIndex:
                return QVariant(QString("Level"));
            case MessageColumnIndex:
                return QVariant(QString("Message"));

            default:
                return QVariant();
        }
    }

    //        if (role == Qt::SizeHintRole) {
    //            switch (section) {
    //                case 0:
    //                    return QVariant(QSize(20, 120));
    //                case 1:
    //                    return QVariant(QSize(20, 120));
    //                case 2:
    //                    return QVariant(QSize(20, 120));
    //                case 3:
    //                    return QVariant(QSize(20, 120));
    //                case 4:
    //                    return QVariant(QSize(20, 100));
    //
    //                default:
    //                    return QVariant();
    //            }
    //        }
    return QVariant();
}


void LoggerViewModel::logEntryDidAddRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &addedRows)
{
    // Faking the view that we are adding new data.
    // Actually when this method is called, the new data has already been
    // added. Anyway, it seems to work
    this->beginInsertRows(QModelIndex(), addedRows.first, addedRows.first + addedRows.second - 1);
    this->endInsertRows();
}

void LoggerViewModel::logEntryDidRemoveRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &removedRows)
{
    // Faking the view that we are removing old data.
    // Actually when this method is called, the old data has already been
    // removed. Anyway, it seems to work
    this->beginRemoveRows(QModelIndex(), removedRows.first, removedRows.first + removedRows.second - 1);
    this->endRemoveRows();

}

