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
    val_trq_choice = settings.value("val_trq_choice", 0).toInt();
    val_vel_choice = settings.value("val_vel_choice", 0).toInt();
    val_pos_custom_step = settings.value("val_pos_custom_step", 1.0).toDouble();
    val_trq_custom_step = settings.value("val_trq_custom_step", 1.0).toDouble();
    val_vel_custom_step = settings.value("val_vel_custom_step", 1.0).toDouble();
    val_vel_limit       = settings.value("velocity_slider_limit", 100.0).toDouble();

    pos_step_validator = new QDoubleValidator(this);
    vel_step_validator = new QDoubleValidator(this);
    trq_step_validator = new QDoubleValidator(this);
    vel_lims_validator = new QDoubleValidator(this);
    vel_lims_validator->setRange(0 , 100);

    ui->pos_step->setValidator(vel_step_validator);
    ui->vel_step->setValidator(vel_step_validator);
    ui->trq_step->setValidator(trq_step_validator);
    ui->vel_limiter->setValidator(vel_lims_validator);

    ui->pos_step->setText(QString("%1").arg(val_pos_custom_step));
    ui->trq_step->setText(QString("%1").arg(val_trq_custom_step));
    ui->vel_step->setText(QString("%1").arg(val_vel_custom_step));
    ui->vel_limiter->setText(QString("%1").arg(val_vel_limit));

    connect(this, SIGNAL(sig_setPosSliderOptionSO(int, double)), parent, SLOT(onSetPosSliderOptionMW(int, double)));
    connect(this, SIGNAL(sig_setVelSliderOptionSO(int, double)), parent, SLOT(onSetVelSliderOptionMW(int, double)));
    connect(this, SIGNAL(sig_setTrqSliderOptionSO(int, double)), parent, SLOT(onSetTrqSliderOptionMW(int, double)));

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
}

sliderOptions::~sliderOptions()
{
    if      (ui->radio_pos_auto->isChecked()) val_pos_choice = 0;
    else if (ui->radio_pos_user->isChecked()) val_pos_choice = 1;
    else if (ui->radio_pos_one->isChecked())  val_pos_choice = 2;

    if      (ui->radio_vel_auto->isChecked()) val_vel_choice = 0;
    else if (ui->radio_vel_user->isChecked()) val_vel_choice = 1;
    else if (ui->radio_vel_one->isChecked())  val_vel_choice = 2;

    if      (ui->radio_trq_auto->isChecked()) val_trq_choice = 0;
    else if (ui->radio_trq_user->isChecked()) val_trq_choice = 1;
    else if (ui->radio_trq_one->isChecked())  val_trq_choice = 2;

    val_pos_custom_step = ui->pos_step->text().toDouble();
    val_vel_custom_step = ui->vel_step->text().toDouble();
    val_trq_custom_step = ui->trq_step->text().toDouble();
    val_vel_limit = ui->vel_limiter->text().toDouble();

    QSettings settings("YARP", "yarpmotorgui");
    settings.setValue("val_pos_choice", val_pos_choice);
    settings.setValue("val_trq_choice", val_trq_choice);
    settings.setValue("val_vel_choice", val_vel_choice);
    settings.setValue("val_pos_custom_step", val_pos_custom_step);
    settings.setValue("val_trq_custom_step", val_trq_custom_step);
    settings.setValue("val_vel_custom_step", val_vel_custom_step);
    settings.setValue("velocity_slider_limit", val_vel_limit);
    emit sig_setPosSliderOptionSO(val_pos_choice, val_pos_custom_step);
    emit sig_setVelSliderOptionSO(val_vel_choice, val_vel_custom_step);
    emit sig_setTrqSliderOptionSO(val_trq_choice, val_trq_custom_step);
    disconnect(this, SIGNAL(sig_setPosSliderOptionSO(int, double)), nullptr, nullptr);
    disconnect(this, SIGNAL(sig_setVelSliderOptionSO(int, double)), nullptr, nullptr);
    disconnect(this, SIGNAL(sig_setTrqSliderOptionSO(int, double)), nullptr, nullptr);

    delete pos_step_validator;
    delete vel_step_validator;
    delete trq_step_validator;
    delete vel_lims_validator;
    delete ui;
}
