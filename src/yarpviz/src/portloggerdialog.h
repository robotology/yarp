#ifndef PORTLOGGERDIALOG_H
#define PORTLOGGERDIALOG_H

#include <QDialog>
#include <yarp/profiler/NetworkProfiler.h>
#include <QTimer>
#include <QTime>

namespace Ui {
class PortLoggerDialog;
}

class PortLoggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortLoggerDialog(yarp::profiler::graph::Graph *graph, QWidget *parent = 0);
    ~PortLoggerDialog();

private slots:
    void addConnections();
    void removeConnections();
    void startStopLoggers();
    void MyTimerSlot();
    void setLogPath();
    void openCons();

private:
    bool saveLog(std::string filename, yarp::os::Bottle* samples);
private:
    Ui::PortLoggerDialog *ui;
    bool isStarted;
    yarp::profiler::graph::Graph *graph;
    QTimer *timer;
    QTime logTime;
};

#endif // PORTLOGGERDIALOG_H
