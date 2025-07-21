/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "dc1394sliderwb.h"
#include "ui_dc1394sliderwb.h"

using namespace yarp::dev;

DC1394SliderWB::DC1394SliderWB(QWidget *parent) :
    DC1394SliderBase(parent),
    ui(new Ui::DC1394SliderWB)
{
    ui->setupUi(this);

    m_bInactive=false;

    m_old_red=m_old_blu=-1.0;
}

DC1394SliderWB::~DC1394SliderWB()
{
    disconnectWidgets();
    delete ui;
}

void DC1394SliderWB::updateSliders()
{
    int rvalue = ui->m_SliderRed->value();
    double rval = (double)rvalue/1000;
    int rw = ui->m_SliderRed->width() - 30;
    double rnewX = ((double)rw/(double)1000) * (double)rvalue;
    ui->lblValueRed->setGeometry(rnewX,0,30,20);
    ui->lblValueRed->setText(QString("%L1").arg(rval,0,'f',3));

    int bvalue = ui->m_SliderBlue->value();
    double bval = (double)bvalue/1000;
    int bw = ui->m_SliderBlue->width() - 30;
    double bnewX = ((double)bw/(double)1000) * (double)bvalue;
    ui->lblValueBlue->setGeometry(bnewX,0,30,20);
    ui->lblValueBlue->setText(QString("%L1").arg(bval,0,'f',3));
}

void DC1394SliderWB::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);

   updateSliders();
}

bool DC1394SliderWB::init(DC1394Thread *controlThread)
{
    this->controlThread = controlThread;

    connect(controlThread,SIGNAL(sliderHasFeatureDone(QObject*,bool)),
           this,SLOT(onHasFeatureDone(QObject*,bool)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderWBRefreshDone(QObject*,bool,bool,bool,bool,bool,bool,double,double)),
            this,SLOT(onRefreshDone(QObject*,bool,bool,bool,bool,bool,bool,double,double)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderWBSetFeatureDC1394Done(QObject*,double,double)),
            this,SLOT(onSliderWBSetFeatureDone(QObject*,double,double)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderRadioAutoDone(QObject*,bool,bool)),
            this,SLOT(onRadioAutoDone(QObject*,bool,bool)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderPowerDone(QObject*,bool,bool,bool,bool)),
            this,SLOT(onPowerDone(QObject*,bool,bool,bool,bool)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderWBOnePushDone(QObject*,double,double)),
            this,SLOT(onOnePushDone(QObject*,double,double)),Qt::QueuedConnection);

    type = SLIDERWB;

    ui->label->setTitle("White Balance");

    connectWidgets();

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    controlThread->doTask(_sliderHasFeature,list);

    return true;

}

void DC1394SliderWB::onHasFeatureDone(QObject *slider, bool hasFeature)
{
    if(slider != this){
        return;
    }

    if(hasFeature){
        return;
    }

    disconnectWidgets();

    m_bInactive=true;
    setVisible(false);

    featureDisabled(this);
}


void DC1394SliderWB::connectWidgets()
{
    connect(ui->m_SliderRed,SIGNAL(valueChanged(int)),this,SLOT(onSliderRedValueChanged(int)));
    connect(ui->m_SliderRed,SIGNAL(sliderReleased()),this,SLOT(onSliderRedReleased()));


    connect(ui->m_SliderBlue,SIGNAL(valueChanged(int)),this,SLOT(onSliderBlueValueChanged(int)));
    connect(ui->m_SliderBlue,SIGNAL(sliderReleased()),this,SLOT(onSliderBlueReleased()));

    connect(ui->m_OnePush,SIGNAL(clicked()),this,SLOT(onOnePushClicked()));
    connect(ui->pRBa,SIGNAL(toggled(bool)),this,SLOT(onRadioAuto(bool)));
    connect(ui->pPwr,SIGNAL(toggled(bool)),this,SLOT(onPower(bool)));
}

void DC1394SliderWB::disconnectWidgets()
{
    disconnect(ui->m_SliderRed,SIGNAL(valueChanged(int)),this,SLOT(onSliderRedValueChanged(int)));
    disconnect(ui->m_SliderRed,SIGNAL(sliderReleased()),this,SLOT(onSliderRedReleased()));
    disconnect(ui->m_SliderBlue,SIGNAL(valueChanged(int)),this,SLOT(onSliderBlueValueChanged(int)));
    disconnect(ui->m_SliderBlue,SIGNAL(sliderReleased()),this,SLOT(onSliderBlueReleased()));

    disconnect(ui->m_OnePush,SIGNAL(clicked()),this,SLOT(onOnePushClicked()));
    disconnect(ui->pRBa,SIGNAL(toggled(bool)),this,SLOT(onRadioAuto(bool)));
    disconnect(ui->pPwr,SIGNAL(toggled(bool)),this,SLOT(onPower(bool)));
}


void DC1394SliderWB::Refresh()
{
    if (m_bInactive){
        return;
    }

    int f = (int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE;
    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant(f));
    controlThread->doTask(_sliderWBRefresh,list);
}

void DC1394SliderWB::onRefreshDone(QObject *slider,bool bON,bool bAuto,bool bHasOnOff,bool bHasAuto,bool bHasManual,bool bHasOnePush,double redVal, double blueVal)
{
    if(slider != this){
        return;
    }
    disconnectWidgets();

    if (bAuto) {
        ui->pRBa->setChecked(true);
    } else {
        ui->pRBm->setChecked(true);
    }

    ui->pPwr->setChecked(bON);

    ui->pPwr->setEnabled(bHasOnOff || bON || bAuto);
    ui->pRBa->setEnabled(bON && bHasAuto);
    ui->pRBm->setEnabled(bON && bHasManual);
    ui->m_SliderRed->setEnabled(bHasManual && !bAuto);
    ui->m_SliderBlue->setEnabled(bHasManual && !bAuto);
    ui->m_OnePush->setEnabled(bON && bHasOnePush);

    m_new_blu = blueVal;
    m_new_red = redVal;

    if (bAuto) {
        ui->pRBa->setChecked(true);
    } else {
        ui->pRBm->setChecked(true);
    }

    if (m_new_blu!=m_old_blu){
        m_old_blu=m_new_blu;
        ui->m_SliderBlue->setValue(m_new_blu * 1000);
        ui->m_SliderBlue->update();
        onSliderBlueValueChanged(m_new_blu * 1000);
    }

    if (m_new_red!=m_old_red){
        m_old_red=m_new_red;
        ui->m_SliderRed->setValue(m_new_red * 1000);
        ui->m_SliderRed->update();
        onSliderRedValueChanged(m_new_red * 1000);
    }

    connectWidgets();
}

void DC1394SliderWB::Propagate()
{
    if (m_bInactive){
        return;
    }

    QVariantList list;
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    list.append(QVariant((double)ui->m_SliderRed->value()/1000));
    list.append(QVariant((double)ui->m_SliderBlue->value()/1000));
    list.append(QVariant(ui->pRBa->isChecked()));
    list.append(QVariant(ui->pPwr->isChecked()));

    controlThread->doTask(_sliderWBPropagate,list);
}

void DC1394SliderWB::onSliderRedReleased()
{
    double val = (double)ui->m_SliderRed->value()/1000;
    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    list.append(QVariant(val));
    list.append(QVariant((double)ui->m_SliderBlue->value()/1000));
    controlThread->doTask(_sliderWBSetFeature,list);
}

void DC1394SliderWB::onSliderRedValueChanged(int value)
{
    double val = (double)value/1000;
    int w = ui->m_SliderRed->width() - 30;
    double newX = ((double)w/(double)1000) * (double)value;
    ui->lblValueRed->setGeometry(newX,0,30,20);
    ui->lblValueRed->setText(QString("%L1").arg(val,0,'f',3));
}

void DC1394SliderWB::onSliderBlueReleased()
{
    double val = (double)ui->m_SliderBlue->value()/1000;
    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    list.append(QVariant((double)ui->m_SliderRed->value()/1000));
    list.append(QVariant(val));
    controlThread->doTask(_sliderWBSetFeature,list);
}

void DC1394SliderWB::onSliderBlueValueChanged(int value)
{
    double val = (double)value/1000;
    int w = ui->m_SliderBlue->width() - 30;
    double newX = ((double)w/(double)1000) * (double)value;
    ui->lblValueBlue->setGeometry(newX,0,30,20);
    ui->lblValueBlue->setText(QString("%L1").arg(val,0,'f',3));
}

void DC1394SliderWB::onSliderWBSetFeatureDone(QObject *slider,double redVal,double blueVal)
{
    if(slider != this){
        return;
    }
    yDebug("white balance %f %f\n", blueVal, redVal);
}

void DC1394SliderWB::onOnePushClicked()
{
    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    controlThread->doTask(_sliderWBOnePush,list);
}

void DC1394SliderWB::onOnePushDone(QObject *slider, double redVal, double blueVal)
{
    if(slider != this){
        return;
    }

    m_new_blu = blueVal;
    m_new_red = redVal;

    disconnectWidgets();
    if (m_new_blu!=m_old_blu){
        m_old_blu=m_new_blu;
        ui->m_SliderBlue->setValue(m_new_blu * 1000);
        onSliderBlueReleased();
    }

    if (m_new_red!=m_old_red){
        m_old_red=m_new_red;
        ui->m_SliderRed->setValue(m_new_red * 1000);
        onSliderRedReleased();
    }

    connectWidgets();

    controlThread->doTask(_reload);
}


void DC1394SliderWB::onRadioAuto(bool toggled)
{
    bool bAuto=toggled;

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    list.append(QVariant(bAuto));
    controlThread->doTask(_sliderRadioAuto,list);
}

void DC1394SliderWB::onRadioAutoDone(QObject *slider,bool bON, bool bAuto)
{
    if(slider != this){
        return;
    }

    ui->m_SliderRed->setEnabled(!bAuto);
    ui->m_SliderBlue->setEnabled(!bAuto);
    yDebug("%s\n", ui->pRBa->isEnabled() ? "auto" : "man");
    controlThread->doTask(_reload);
}

void DC1394SliderWB::onPower(bool checked)
{
    bool bON=checked;

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)cameraFeature_id_t::YARP_FEATURE_WHITE_BALANCE));
    list.append(QVariant(bON));
    controlThread->doTask(_sliderPower,list);
}

void DC1394SliderWB::onPowerDone(QObject *slider, bool bON,bool hasAuto, bool hasManual, bool hasOnePush)
{
    if(slider != this){
        return;
    }

    ui->pRBa->setEnabled(bON && hasAuto);
    ui->pRBm->setEnabled(bON && hasManual);
    ui->m_SliderRed->setEnabled(bON && ui->pRBm->isChecked());
    ui->m_SliderBlue->setEnabled(bON && ui->pRBm->isChecked());
    ui->m_OnePush->setEnabled(bON && hasOnePush);
    yDebug("power %s\n", ui->pPwr->isChecked() ? "on" : "off");

    controlThread->doTask(_reload);
}

void DC1394SliderWB::set_value(double blue,double red)
{
    ui->m_SliderBlue->setValue(blue * 1000);
    ui->m_SliderRed->setValue(red * 1000);

    onSliderBlueReleased();
    onSliderRedReleased();
}
