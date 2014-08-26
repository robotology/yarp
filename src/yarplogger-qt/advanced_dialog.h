#ifndef ADVANCED_DIALOG_H
#define ADVANCED_DIALOG_H

#include <QDialog>
#include <qabstractbutton>
#include <yarp/os/YarprunLogger.h>

namespace Ui {
class advanced_dialog;
}

class advanced_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit advanced_dialog(yarp::os::YarprunLogger::LoggerEngine* logger, QWidget *parent = 0);
    ~advanced_dialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_radio_log_unlimited_clicked();

    void on_radio_log_limited_clicked();

    void on_radio_ports_unlimited_clicked();

    void on_radio_ports_limited_clicked();

private:
    Ui::advanced_dialog *ui;
    yarp::os::YarprunLogger::LoggerEngine* theLogger;
};

#endif // ADVANCED_DIALOG_H
