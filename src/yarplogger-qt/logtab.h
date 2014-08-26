#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex>
#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <yarp/os/YarprunLogger.h>
#include "messagewidget.h"

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
    explicit LogTab(yarp::os::YarprunLogger::LoggerEngine*  _theLogger, LogWidget* _system_message, std::string _portName, QWidget *parent = 0, int refreshRate=100);
    ~LogTab();

private:
    Ui::LogTab *ui;
    std::string                            portName;
    yarp::os::YarprunLogger::LoggerEngine* theLogger;
    LogWidget*                             system_message;
    QMutex                                 mutex;
    bool                                   displayYarprunTimestamp_enabled;
    bool                                   displayLocalTimestamp_enabled;
    bool                                   displayErrorLevel_enabled;
    bool                                   displayColors_enabled;
    bool                                   displayGrid_enabled;

private slots:
    void updateLog(bool from_beginning=false);

public:
    QTimer                  *logTimer;
    QStandardItemModel      *model_logs;
    QSortFilterProxyModel   *proxyModelButtons;
    QSortFilterProxyModel   *proxyModelSearch;
    void                    displayYarprunTimestamp  (bool enabled);
    void                    displayLocalTimestamp    (bool enabled);
    void                    displayErrorLevel (bool enabled);
    void                    displayColors     (bool enabled);
    void                    displayGrid       (bool enabled);
    void                    clear_model_logs();
};

#endif // LOGTAB_H
