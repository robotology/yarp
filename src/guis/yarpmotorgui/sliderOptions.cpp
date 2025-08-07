/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "sliderOptions.h"
#include "ui_sliderOptions.h"
#include <QSettings>

sliderOptions::sliderOptions( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sliderOptions)
{
    ui->setupUi(this);

    QString title = QString("SliderOptions");
    setWindowTitle(title);

    QSettings settings("YARP", "yarpmotorgui");
    val_pos_choice = settings.value("val_pos_choice", 0).toInt();
    val_vel_choice = settings.value("val_vel_choice", 0).toInt();
    val_acc_choice = settings.value("val_acc_choice", 0).toInt();
    val_trq_choice = settings.value("val_trq_choice", 0).toInt();
    val_cur_choice = settings.value("val_cur_choice", 0).toInt();

    val_pos_digits = settings.value("num_of_pos_decimals", 3).toInt();
    val_vel_digits = settings.value("num_of_vel_decimals", 3).toInt();
    val_acc_digits = settings.value("num_of_acc_decimals", 3).toInt();
    val_trq_digits = settings.value("num_of_trq_decimals", 3).toInt();
    val_cur_digits = settings.value("num_of_cur_decimals", 3).toInt();

    val_pos_custom_step = settings.value("val_pos_custom_step", 1.0).toDouble();
    val_vel_custom_step = settings.value("val_vel_custom_step", 1.0).toDouble();
    val_acc_custom_step = settings.value("val_acc_custom_step", 1.0).toDouble();
    val_trq_custom_step = settings.value("val_trq_custom_step", 1.0).toDouble();
    val_cur_custom_step = settings.value("val_cur_custom_step", 1.0).toDouble();

    val_vel_limit       = settings.value("velocity_slider_limit", 100.0).toDouble();
    val_acc_limit       = settings.value("acceleration_slider_limit", 100.0).toDouble();

    pos_digits_validator = new QIntValidator(this);
    vel_digits_validator = new QIntValidator(this);
    acc_digits_validator = new QIntValidator(this);
    trq_digits_validator = new QIntValidator(this);
    cur_digits_validator = new QIntValidator(this);

    pos_step_validator = new QDoubleValidator(this);
    vel_step_validator = new QDoubleValidator(this);
    acc_step_validator = new QDoubleValidator(this);
    trq_step_validator = new QDoubleValidator(this);
    cur_step_validator = new QDoubleValidator(this);

    vel_lims_validator = new QDoubleValidator(this);
    vel_lims_validator->setRange(0 , 100);
    acc_lims_validator = new QDoubleValidator(this);
    acc_lims_validator->setRange(0 , 100);

    ui->pos_decimal_digits->setValidator(pos_digits_validator);
    ui->vel_decimal_digits->setValidator(vel_digits_validator);
    ui->acc_decimal_digits->setValidator(acc_digits_validator);
    ui->trq_decimal_digits->setValidator(trq_digits_validator);
    //ui->cur_decimal_digits->setValidator(cur_digits_validator);

    ui->pos_step->setValidator(pos_step_validator);
    ui->vel_step->setValidator(vel_step_validator);
    ui->acc_step->setValidator(acc_step_validator);
    ui->trq_step->setValidator(trq_step_validator);
    //ui->cur_step->setValidator(cur_step_validator);

    ui->vel_limiter->setValidator(vel_lims_validator);
    ui->acc_limiter->setValidator(acc_lims_validator);

    ui->pos_decimal_digits->setText(QString("%1").arg(val_pos_digits));
    ui->vel_decimal_digits->setText(QString("%1").arg(val_vel_digits));
    ui->acc_decimal_digits->setText(QString("%1").arg(val_acc_digits));
    ui->trq_decimal_digits->setText(QString("%1").arg(val_trq_digits));
    //ui->cur_decimal_digits->setText(QString("%1").arg(val_cur_digits));

    ui->pos_step->setText(QString("%1").arg(val_pos_custom_step));
    ui->vel_step->setText(QString("%1").arg(val_vel_custom_step));
    ui->acc_step->setText(QString("%1").arg(val_acc_custom_step));
    ui->trq_step->setText(QString("%1").arg(val_trq_custom_step));
    //ui->cur_step->setText(QString("%1").arg(val_cur_custom_step));

    ui->vel_limiter->setText(QString("%1").arg(val_vel_limit));
    ui->acc_limiter->setText(QString("%1").arg(val_acc_limit));

    connect(this, SIGNAL(sig_setPosSliderOptionSO(int, double, int)), parent, SLOT(onSetPosSliderOptionMW(int, double, int)));
    connect(this, SIGNAL(sig_setVelSliderOptionSO(int, double, int)), parent, SLOT(onSetVelSliderOptionMW(int, double, int)));
    connect(this, SIGNAL(sig_setAccSliderOptionSO(int, double, int)), parent, SLOT(onSetAccSliderOptionMW(int, double, int)));
    connect(this, SIGNAL(sig_setTrqSliderOptionSO(int, double, int)), parent, SLOT(onSetTrqSliderOptionMW(int, double, int)));
    connect(this, SIGNAL(sig_setCurSliderOptionSO(int, double, int)), parent, SLOT(onSetCurSliderOptionMW(int, double, int)));

    switch (val_pos_choice)
    {
        case 0:
            ui->radio_pos_auto->setChecked(true);
        break;
        case 1:
            ui->radio_pos_user->setChecked(true);
        break;
        case 2:
            ui->radio_pos_one->setChecked(true);
        break;
        default:
            ui->radio_pos_auto->setChecked(true);
        break;
    }
    switch (val_vel_choice)
    {
        case 0:
            ui->radio_vel_auto->setChecked(true);
        break;
        case 1:
            ui->radio_vel_user->setChecked(true);
        break;
        case 2:
            ui->radio_vel_one->setChecked(true);
        break;
        default:
            ui->radio_vel_auto->setChecked(true);
        break;
    }
    switch (val_acc_choice)
    {
        case 0:
            ui->radio_acc_auto->setChecked(true);
        break;
        case 1:
            ui->radio_acc_user->setChecked(true);
        break;
        case 2:
            ui->radio_acc_one->setChecked(true);
        break;
        default:
            ui->radio_acc_auto->setChecked(true);
        break;
    }
    switch (val_trq_choice)
    {
        case 0:
            ui->radio_trq_auto->setChecked(true);
        break;
        case 1:
            ui->radio_trq_user->setChecked(true);
        break;
        case 2:
            ui->radio_trq_one->setChecked(true);
        break;
        default:
            ui->radio_trq_auto->setChecked(true);
        break;
    }
    /* switch (val_cur_choice)
    {
        case 0:
            ui->radio_cur_auto->setChecked(true);
        break;
        case 1:
            ui->radio_cur_user->setChecked(true);
        break;
        case 2:
            ui->radio_cur_one->setChecked(true);
        break;
        default:
            ui->radio_cur_auto->setChecked(true);
        break;
    }
    */
}

sliderOptions::~sliderOptions()
{
    //send all values on windows closure, by emitting signals, then destory the window

    if (ui->radio_pos_auto->isChecked()) {
        val_pos_choice = 0;
    } else if (ui->radio_pos_user->isChecked()) {
        val_pos_choice = 1;
    } else if (ui->radio_pos_one->isChecked()) {
        val_pos_choice = 2;
    }

    if (ui->radio_vel_auto->isChecked()) {
        val_vel_choice = 0;
    } else if (ui->radio_vel_user->isChecked()) {
        val_vel_choice = 1;
    } else if (ui->radio_vel_one->isChecked()) {
        val_vel_choice = 2;
    }

    if (ui->radio_acc_auto->isChecked()) {
        val_acc_choice = 0;
    } else if (ui->radio_acc_user->isChecked()) {
        val_acc_choice = 1;
    } else if (ui->radio_acc_one->isChecked()) {
        val_acc_choice = 2;
    }

    if (ui->radio_trq_auto->isChecked()) {
        val_trq_choice = 0;
    } else if (ui->radio_trq_user->isChecked()) {
        val_trq_choice = 1;
    } else if (ui->radio_trq_one->isChecked()) {
        val_trq_choice = 2;
    }

/*
    if (ui->radio_cur_auto->isChecked()) {
        val_cur_choice = 0;
    } else if (ui->radio_cur_user->isChecked()) {
        val_cur_choice = 1;
    } else if (ui->radio_cur_one->isChecked()) {
        val_cur_choice = 2;
    }
*/

    val_pos_custom_step = ui->pos_step->text().toDouble();
    val_vel_custom_step = ui->vel_step->text().toDouble();
    val_acc_custom_step = ui->acc_step->text().toDouble();
    val_trq_custom_step = ui->trq_step->text().toDouble();
    //val_cur_custom_step = ui->cur_step->text().toDouble();

    val_vel_limit = ui->vel_limiter->text().toDouble();
    val_acc_limit = ui->acc_limiter->text().toDouble();

    val_pos_digits = ui->pos_decimal_digits->text().toInt();
    val_vel_digits = ui->vel_decimal_digits->text().toInt();
    val_acc_digits = ui->acc_decimal_digits->text().toInt();
    val_trq_digits = ui->trq_decimal_digits->text().toInt();
    //val_cur_digits = ui->cur_decimal_digits->text().toInt();

    QSettings settings("YARP", "yarpmotorgui");
    settings.setValue("val_pos_choice", val_pos_choice);
    settings.setValue("val_vel_choice", val_vel_choice);
    settings.setValue("val_acc_choice", val_acc_choice);
    settings.setValue("val_trq_choice", val_trq_choice);
    settings.setValue("val_cur_choice", val_cur_choice);

    settings.setValue("val_pos_custom_step", val_pos_custom_step);
    settings.setValue("val_vel_custom_step", val_vel_custom_step);
    settings.setValue("val_acc_custom_step", val_acc_custom_step);
    settings.setValue("val_trq_custom_step", val_trq_custom_step);
    settings.setValue("val_cur_custom_step", val_cur_custom_step);

    settings.setValue("velocity_slider_limit", val_vel_limit);
    settings.setValue("acceleration_slider_limit", val_acc_limit);

    settings.setValue("num_of_pos_decimals", val_pos_digits);
    settings.setValue("num_of_vel_decimals", val_vel_digits);
    settings.setValue("num_of_acc_decimals", val_acc_digits);
    settings.setValue("num_of_trq_decimals", val_trq_digits);
    settings.setValue("num_of_cur_decimals", val_cur_digits);

    emit sig_setPosSliderOptionSO(val_pos_choice, val_pos_custom_step, val_pos_digits);
    emit sig_setVelSliderOptionSO(val_vel_choice, val_vel_custom_step, val_vel_digits);
    emit sig_setAccSliderOptionSO(val_acc_choice, val_acc_custom_step, val_acc_digits);
    emit sig_setTrqSliderOptionSO(val_trq_choice, val_trq_custom_step, val_trq_digits);
    emit sig_setCurSliderOptionSO(val_cur_choice, val_cur_custom_step, val_cur_digits);

    disconnect(this, SIGNAL(sig_setPosSliderOptionSO(int, double, int)), nullptr, nullptr);
    disconnect(this, SIGNAL(sig_setVelSliderOptionSO(int, double, int)), nullptr, nullptr);
    disconnect(this, SIGNAL(sig_setAccSliderOptionSO(int, double, int)), nullptr, nullptr);
    disconnect(this, SIGNAL(sig_setTrqSliderOptionSO(int, double, int)), nullptr, nullptr);
    disconnect(this, SIGNAL(sig_setCurSliderOptionSO(int, double, int)), nullptr, nullptr);

    delete pos_digits_validator;
    delete vel_digits_validator;
    delete acc_digits_validator;
    delete trq_digits_validator;
    delete cur_digits_validator;

    delete pos_step_validator;
    delete vel_step_validator;
    delete acc_step_validator;
    delete trq_step_validator;
    delete cur_step_validator;

    delete vel_lims_validator;
    delete acc_lims_validator;

    delete ui;
}
