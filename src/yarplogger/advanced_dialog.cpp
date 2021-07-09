/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "advanced_dialog.h"
#include "ui_advanced_dialog.h"
#include <QIntValidator>

advanced_dialog::advanced_dialog(yarp::yarpLogger::LoggerEngine* logger, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::advanced_dialog)
{
    theLogger = logger;
    ui->setupUi(this);
    ui->log_max_size->setValidator   ( new QIntValidator(100, 100000, this) );
    ui->ports_max_size->setValidator ( new QIntValidator(10,  1000,   this) );
    ui->refresh_rate->setValidator   ( new QIntValidator(10,  100000, this) );
    ui->cap_error->setChecked(theLogger->get_listen_option(yarp::yarpLogger::LogLevel(yarp::yarpLogger::LOGLEVEL_ERROR)));
    ui->cap_warning->setChecked(theLogger->get_listen_option(yarp::yarpLogger::LogLevel(yarp::yarpLogger::LOGLEVEL_WARNING)));
    ui->cap_debug->setChecked(theLogger->get_listen_option(yarp::yarpLogger::LogLevel(yarp::yarpLogger::LOGLEVEL_DEBUG)));
    ui->cap_info->setChecked(theLogger->get_listen_option(yarp::yarpLogger::LogLevel(yarp::yarpLogger::LOGLEVEL_INFO)));
    ui->cap_trace->setChecked(theLogger->get_listen_option(yarp::yarpLogger::LogLevel(yarp::yarpLogger::LOGLEVEL_TRACE)));
    ui->cap_unformatted->setChecked(theLogger->get_listen_option(yarp::yarpLogger::LogLevel(yarp::yarpLogger::LOGLEVEL_UNDEFINED)));

    bool enable_log_lines_limit;
    int  log_lines_limit;
    theLogger->get_log_lines_max_size(enable_log_lines_limit,log_lines_limit);
    if (enable_log_lines_limit) ui->radio_log_limited->setChecked(true);
    else  ui->radio_log_unlimited->setChecked(true);
    char log_lines_limit_s [20];
    sprintf(log_lines_limit_s, "%d", log_lines_limit);
    ui->log_max_size->setText(log_lines_limit_s);

    bool enable_log_ports_limit;
    int  log_ports_limit;
    theLogger->get_log_list_max_size(enable_log_ports_limit,log_ports_limit);
    if (enable_log_ports_limit) ui->radio_ports_limited->setChecked(true);
    else  ui->radio_ports_unlimited->setChecked(true);
    char log_ports_limit_s [20];
    sprintf(log_ports_limit_s, "%d", log_ports_limit);
    ui->ports_max_size->setText(log_ports_limit_s);
}

advanced_dialog::~advanced_dialog()
{
    delete ui;
}

void advanced_dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    theLogger->set_listen_option(yarp::yarpLogger::LOGLEVEL_ERROR,ui->cap_error->checkState());
    theLogger->set_listen_option(yarp::yarpLogger::LOGLEVEL_WARNING,ui->cap_warning->checkState());
    theLogger->set_listen_option(yarp::yarpLogger::LOGLEVEL_DEBUG,ui->cap_debug->checkState());
    theLogger->set_listen_option(yarp::yarpLogger::LOGLEVEL_INFO,ui->cap_info->checkState());
    theLogger->set_listen_option(yarp::yarpLogger::LOGLEVEL_TRACE,ui->cap_trace->checkState());
    theLogger->set_listen_option(yarp::yarpLogger::LOGLEVEL_UNDEFINED,ui->cap_unformatted->checkState());

    QString s_lines    = ui->log_max_size->text();
    int size_log_lines = atoi (s_lines.toStdString().c_str());
    if (ui->radio_log_limited->isChecked())
    {
        theLogger->set_log_lines_max_size(true, size_log_lines);
    }
    if (ui->radio_log_unlimited->isChecked())
    {
        theLogger->set_log_lines_max_size(false,size_log_lines);
    }

    QString s_ports = ui->ports_max_size->text();
    int size_ports  = atoi (s_ports.toStdString().c_str());
    if (ui->radio_ports_limited->isChecked())
    {
        theLogger->set_log_list_max_size(true, size_ports);
    }
    if (ui->radio_ports_unlimited->isChecked())
    {
        theLogger->set_log_list_max_size(false,size_ports);
    }

//     QString s_refresh_rate = ui->refresh_rate->text();
//     int refresh_rate  = atoi (s_refresh_rate.toStdString().c_str());

}

void advanced_dialog::on_radio_log_unlimited_clicked()
{

}

void advanced_dialog::on_radio_log_limited_clicked()
{

}

void advanced_dialog::on_radio_ports_unlimited_clicked()
{

}

void advanced_dialog::on_radio_ports_limited_clicked()
{

}
