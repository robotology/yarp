#include "advanced_dialog.h"
#include "ui_advanced_dialog.h"

advanced_dialog::advanced_dialog(yarp::os::YarprunLogger::LoggerEngine* logger, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::advanced_dialog)
{
    theLogger = logger;
    ui->setupUi(this);

    ui->cap_error->setChecked(theLogger->get_listen_option(yarp::os::YarprunLogger::LOGLEVEL_ERROR));
    ui->cap_warning->setChecked(theLogger->get_listen_option(yarp::os::YarprunLogger::LOGLEVEL_WARNING));
    ui->cap_debug->setChecked(theLogger->get_listen_option(yarp::os::YarprunLogger::LOGLEVEL_DEBUG));
    ui->cap_info->setChecked(theLogger->get_listen_option(yarp::os::YarprunLogger::LOGLEVEL_INFO));
    ui->cap_unformatted->setChecked(theLogger->get_listen_option(yarp::os::YarprunLogger::LOGLEVEL_UNDEFINED));
}

advanced_dialog::~advanced_dialog()
{
    delete ui;
}

void advanced_dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    theLogger->set_listen_option(yarp::os::YarprunLogger::LOGLEVEL_ERROR,ui->cap_error->checkState());
    theLogger->set_listen_option(yarp::os::YarprunLogger::LOGLEVEL_WARNING,ui->cap_warning->checkState());
    theLogger->set_listen_option(yarp::os::YarprunLogger::LOGLEVEL_DEBUG,ui->cap_debug->checkState());
    theLogger->set_listen_option(yarp::os::YarprunLogger::LOGLEVEL_INFO,ui->cap_info->checkState());
    theLogger->set_listen_option(yarp::os::YarprunLogger::LOGLEVEL_UNDEFINED,ui->cap_unformatted->checkState());
}
