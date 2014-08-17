#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex.h>
#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <yarp/os/YarprunLogger.h>

const std::string ERROR_STRING   = "ERROR";
const std::string WARNING_STRING = "WARNING";
const std::string INFO_STRING    = "INFO";
const std::string DEBUG_STRING   = "DEBUG";

namespace Ui {
class LogTab;
}

class LogTab : public QFrame
{
    Q_OBJECT

public:
    explicit LogTab(yarp::os::YarprunLogger::LoggerEngine*  _theLogger, std::string _portName, QWidget *parent = 0);
    ~LogTab();

private:
    Ui::LogTab *ui;
    std::string                            portName;
    yarp::os::YarprunLogger::LoggerEngine* theLogger;
    QMutex                                 mutex;

private slots:
    void updateLog(bool from_beginning=false);

public:
    QTimer                  *logTimer;
    QStandardItemModel      *model_logs;
    QSortFilterProxyModel   *proxyModelButtons;
    QSortFilterProxyModel   *proxyModelSearch;
    bool                     displayTimestamp;

    void                     clear_model_logs();
};

#endif // LOGTAB_H
