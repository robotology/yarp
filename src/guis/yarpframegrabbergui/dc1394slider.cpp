/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "dc1394slider.h"
#include "ui_dc1394slider.h"

DC1394Slider::DC1394Slider(QWidget *parent) :
    DC1394SliderBase(parent),
    ui(new Ui::DC1394Slider)
{
    ui->setupUi(this);


    m_bInactive=false;

    m_old_value=-1.0;
    pressed = false;

}

DC1394Slider::~DC1394Slider()
{
    disconnectWidgets();
    delete ui;
}

void DC1394Slider::updateSliders()
{
    int value = ui->m_Slider->value();
    double val = (double)value/1000;
    int w = ui->m_Slider->width() - 30;
    double newX = ((double)w/(double)1000) * (double)value;
    ui->lblValue->setGeometry(newX,0,30,20);
    ui->lblValue->setText(QString("%L1").arg(val,0,'f',3));
}

void DC1394Slider::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);

   updateSliders();
}

bool DC1394Slider::init(yarp::dev::cameraFeature_id_t feature, char* label, DC1394Thread *controlThread)
{
    m_Feature=feature;
    this->controlThread = controlThread;

    connect(controlThread,SIGNAL(sliderHasFeatureDone(QObject*,bool)),
           this,SLOT(onHasFeatureDone(QObject*,bool)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderRefreshDone(QObject*,bool,bool,bool,bool,bool,bool,double)),
            this,SLOT(onRefreshDone(QObject*,bool,bool,bool,bool,bool,bool,double)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderSetFeatureDC1394Done(QObject*,double)),
            this,SLOT(onSliderSetFeatureDone(QObject*,double)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderRadioAutoDone(QObject*,bool,bool)),
            this,SLOT(onRadioAutoDone(QObject*,bool,bool)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderPowerDone(QObject*,bool,bool,bool,bool)),
            this,SLOT(onPowerDone(QObject*,bool,bool,bool,bool)),Qt::QueuedConnection);

    connect(controlThread,SIGNAL(sliderOnePushDone(QObject*,double)),
            this,SLOT(onOnePushDone(QObject*,double)),Qt::QueuedConnection);

    type = SLIDER;
    m_Name=label;

    ui->label->setTitle(m_Name);

    connectWidgets();

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)m_Feature));
    controlThread->doTask(_sliderHasFeature,list);

    return true;
}

void DC1394Slider::onHasFeatureDone(QObject *slider, bool hasFeature)
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

void DC1394Slider::connectWidgets()
{
    connect(ui->m_Slider,SIGNAL(valueChanged(int)),this,SLOT(onSliderValueChanged(int)));
    connect(ui->m_Slider,SIGNAL(sliderPressed()),this,SLOT(onSliderPressed()));
    connect(ui->m_Slider,SIGNAL(sliderReleased()),this,SLOT(onSliderReleased()));

    connect(ui->m_OnePush,SIGNAL(clicked()),this,SLOT(onOnePushClicked()));
    connect(ui->pRBa,SIGNAL(toggled(bool)),this,SLOT(onRadioAuto(bool)));
    //connect(ui->pRBm,SIGNAL(toggled(bool)),this,SLOT(onRadioManual(bool)));
    connect(ui->pPwr,SIGNAL(toggled(bool)),this,SLOT(onPower(bool)));
}

void DC1394Slider::disconnectWidgets()
{
    disconnect(ui->m_Slider,SIGNAL(valueChanged(int)),this,SLOT(onSliderValueChanged(int)));
    disconnect(ui->m_Slider,SIGNAL(sliderPressed()),this,SLOT(onSliderPressed()));
    disconnect(ui->m_Slider,SIGNAL(sliderReleased()),this,SLOT(onSliderReleased()));
    disconnect(ui->m_OnePush,SIGNAL(clicked()),this,SLOT(onOnePushClicked()));
    disconnect(ui->pRBa,SIGNAL(toggled(bool)),this,SLOT(onRadioAuto(bool)));
    //disconnect(ui->pRBm,SIGNAL(toggled(bool)),this,SLOT(onRadioManual(bool)));
    disconnect(ui->pPwr,SIGNAL(toggled(bool)),this,SLOT(onPower(bool)));
}


void DC1394Slider::Refresh()
{
    if (m_bInactive){
        return;
    }

    int f = (int)m_Feature;
    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant(f));
    controlThread->doTask(_sliderRefresh,list);
}

void DC1394Slider::onRefreshDone(QObject *slider,bool bON,bool bAuto,bool bHasOnOff,bool bHasAuto,bool bHasManual,bool bHasOnePush,double val)
{
    if(slider != this){
        return;
    }
    disconnectWidgets();

    ui->pPwr->setChecked(bON);
    ui->pPwr->setEnabled(bHasOnOff || bON);
    ui->pRBa->setEnabled(bON && bHasAuto);
    ui->pRBm->setEnabled(bON && bHasManual);

    ui->m_Slider->setEnabled(bHasManual && !bAuto);
    ui->lblValue->setEnabled(bHasManual && !bAuto);
    ui->m_OnePush->setEnabled(bON && bHasOnePush);    // why setEnabled(false) is different from setDisable(true)?

    if (bAuto) {
        ui->pRBa->setChecked(true);
    } else {
        ui->pRBm->setChecked(true);
    }

    if (m_old_value != (m_new_value=val)){
        m_old_value=m_new_value;
        ui->m_Slider->setValue(m_new_value * 1000);
        ui->m_Slider->update();
        onSliderValueChanged(m_new_value * 1000);
    }

    connectWidgets();
}

void DC1394Slider::Propagate()
{
    if (m_bInactive){
        return;
    }

    QVariantList list;
    list.append(QVariant((int)m_Feature));
    list.append(QVariant((double)ui->m_Slider->value() / 1000));
    list.append(QVariant(ui->pRBa->isChecked()));
    list.append(QVariant(ui->pPwr->isChecked()));

    controlThread->doTask(_sliderPropagate,list);
}

void DC1394Slider::onSliderPressed()
{
    pressed = true;
}

void DC1394Slider::onSliderReleased()
{

    pressed = false;
    double val = (double)ui->m_Slider->value()/1000;

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)m_Feature));
    list.append(QVariant(val));
    controlThread->doTask(_sliderSetFeature,list);
}

void DC1394Slider::onSliderValueChanged(int value)
{
    double val = (double)value/1000;
    int w = ui->m_Slider->width() - 30;
    double newX = ((double)w/(double)1000) * (double)value;
    ui->lblValue->setGeometry(newX,0,30,20);
    ui->lblValue->setText(QString("%L1").arg(val,0,'f',3));
}

void DC1394Slider::onSliderSetFeatureDone(QObject *slider, double val)
{
    if(slider != this){
        return;
    }
    yDebug("%s new value %lf\n",m_Name.toLatin1().data(),val);
}

void DC1394Slider::onOnePushClicked()
{

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)m_Feature));
    controlThread->doTask(_sliderOnePush,list);
}

void DC1394Slider::onOnePushDone(QObject *slider, double val)
{
    if(slider != this){
        return;
    }

    disconnectWidgets();
    if (m_old_value!=(m_new_value=val)){
        m_old_value=m_new_value;
        ui->m_Slider->setValue(m_new_value * 1000);
        onSliderReleased();
    }
    connectWidgets();

    controlThread->doTask(_reload);
}

void DC1394Slider::onRadioAuto(bool toggled)
{
    bool bAuto=toggled;

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)m_Feature));
    list.append(QVariant(bAuto));
    controlThread->doTask(_sliderRadioAuto,list);
}

void DC1394Slider::onRadioAutoDone(QObject *slider,bool bON, bool bAuto)
{
    if(slider != this){
        return;
    }

    ui->m_Slider->setEnabled(bON && !bAuto);
    ui->lblValue->setEnabled(bON && !bAuto);
    yDebug("%s\n",ui->pRBa->isChecked() ? "auto":"man");

    controlThread->doTask(_reload);
}

void DC1394Slider::onPower(bool checked)
{
    bool bON=checked;

    QVariantList list;
    list.append(qVariantFromValue((void*)this));
    list.append(QVariant((int)m_Feature));
    list.append(QVariant(bON));
    controlThread->doTask(_sliderPower,list);
}

void DC1394Slider::onPowerDone(QObject *slider, bool bON,bool hasAuto, bool hasManual, bool hasOnePush)
{
    if(slider != this){
        return;
    }

    ui->pRBa->setEnabled(bON && hasAuto);
    ui->pRBm->setEnabled(bON && hasManual);
    ui->m_Slider->setEnabled(bON && ui->pRBm->isChecked());
    ui->lblValue->setEnabled(bON && ui->pRBm->isChecked());
    ui->m_OnePush->setEnabled(bON && hasOnePush);    // why setEnabled(false) is different from setDisable(true)?
    yDebug("power %s\n", ui->pPwr->isChecked() ? "on" : "off");

    controlThread->doTask(_reload);
}

void DC1394Slider::set_value(double val)
{
    ui->m_Slider->setValue(val * 1000);
    onSliderValueChanged(val * 1000);
    onSliderReleased();
}
