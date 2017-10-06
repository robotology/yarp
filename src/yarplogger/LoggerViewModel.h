#ifndef YARP_LOGGER_LOGGERVIEWMODEL_H
#define YARP_LOGGER_LOGGERVIEWMODEL_H

#include <yarp/logger/YarpLogger.h>

#include <QAbstractTableModel>

namespace yarp {
    namespace yarpLogger {
        class LoggerEngine;
    }
}

class LoggerViewModel
: public QAbstractTableModel
, public yarp::yarpLogger::LogEntryObserver
{
public:
    LoggerViewModel(yarp::yarpLogger::LoggerEngine& loggerEngine,
                    const std::string& portName);
    virtual ~LoggerViewModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual void logEntryDidAddRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &addedRows);
    virtual void logEntryDidRemoveRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &removedRows);
    

private:
    yarp::yarpLogger::LoggerEngine& m_loggerEngine;
    std::string m_portName;
    std::list<yarp::yarpLogger::MessageEntry> m_logMessages;
    yarp::yarpLogger::LogEntry* m_observedEntry;

public:
    // public constants
    static const size_t YarpRunTimeColumnIndex;
    static const size_t LocalTimeColumnIndex;
    static const size_t LogLevlColumnIndex;
    static const size_t MessageColumnIndex;
};



#endif /* end of include guard: YARP_LOGGER_LOGGERVIEWMODEL_H */
