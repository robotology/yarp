#ifndef LOGTAB_H
#define LOGTAB_H

#include <QFrame>
#include <QStringListModel>
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

private slots:
    void updateLog();

public:
    QTimer                  *logTimer;
    QStringListModel        *logs;
    QSortFilterProxyModel   *proxyModel;

};

#endif // LOGTAB_H
