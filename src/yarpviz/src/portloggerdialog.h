#ifndef PORTLOGGERDIALOG_H
#define PORTLOGGERDIALOG_H

#include <QDialog>
#include <ggraph.h>
#include <QTimer>
#include <QTime>

namespace Ui {
class PortLoggerDialog;
}

class PortLoggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortLoggerDialog(yarp::graph::Graph *graph, QWidget *parent = 0);
    ~PortLoggerDialog();

private slots:
    void addConnections();
    void removeConnections();
    void startStopLoggers();
    void MyTimerSlot();
    void setLogPath();

private:
    bool saveLog(std::string filename, yarp::os::Bottle* samples);
private:
    bool isStarted;
    yarp::graph::Graph *graph;
    Ui::PortLoggerDialog *ui;
    QTimer *timer;
    QTime logTime;
};

#endif // PORTLOGGERDIALOG_H
