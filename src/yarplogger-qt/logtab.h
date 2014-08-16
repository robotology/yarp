#ifndef LOGTAB_H
#define LOGTAB_H

#include <QMutex.h>
#include <QFrame>
#include <QStandardItemModel>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <yarp/os/YarprunLogger.h>

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
    QSortFilterProxyModel   *proxyModel;
    bool                     displayTimestamp;

    void                     clear_model_logs();
};

#endif // LOGTAB_H
