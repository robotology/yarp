/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "sliderWithTarget.h"
#include <QKeyEvent>
#include <QPainter>
#include <QStyle>
#include <cmath>

//just because old VS2010 does not implement round() function in cmath
double my_round(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

SliderWithTarget::SliderWithTarget(QWidget * parent, bool _hasTargetOption) : QSlider(Qt::Horizontal, nullptr)
{
    sliderLabel = new QLabel(this);
    sliderLabel->setObjectName(QStringLiteral("sliderLabel"));
    sliderLabel->setGeometry(QRect(0, 0, 41, 20));
    sliderLabel->setMinimumSize(QSize(40, 20));
    sliderLabel->setMaximumSize(QSize(16777215, 20));
    sliderLabel->setAlignment(Qt::AlignCenter);
    sliderLabel->setStyleSheet("background-color: rgba(0,0,0,0%)");

    hasTargetOption = _hasTargetOption;
    enableViewTarget = true;
    enableViewLabel = true;
    disableClickOutOfHandle = true;
    target=-1e50; //default value is intensionally set out of the slider
    width_at_target = this->width() - 30;
    sliderStep=1;
    isDouble = false;
}

void SliderWithTarget::setSliderStep(double step)
{
    sliderStep = step;
}

double SliderWithTarget::getSliderStep()
{
    return sliderStep;
}

void SliderWithTarget::setIsDouble(bool b)
{
    isDouble = b;
}

bool SliderWithTarget::getIsDouble()
{
    return isDouble;
}

void SliderWithTarget::setValue(double val)
{
    if (isDouble)
    {
        //QSlider::setValue(round(val*sliderStep));
        QSlider::setValue(my_round(val*sliderStep));
    }
    else
    {
        QSlider::setValue(val);
    }
}

void SliderWithTarget::updateSliderTarget(double t)
{
    target = t;
    width_at_target= this->width()-30;
    this->update(); //force the reapint of the widget
}

void SliderWithTarget::resetTarget()
{
    target = -1e50;
    width_at_target = this->width() - 30;
    this->update(); //force the reapint of the widget
}

void SliderWithTarget::paintEvent(QPaintEvent *e)
{
    QSlider::paintEvent(e);
    if (hasTargetOption && enableViewTarget)
    {
        QPainter p(this);
        double current_width = this->width()-30;
        QRect r(target / width_at_target * current_width, 17, 30, 15);
        p.fillRect(r, QBrush(QColor(128, 128, 255, 128)));
        p.drawRect(r);
    }
    if (enableViewLabel)
    {
        double  value = this->value();
        int w = this->width() - 30;
        int totValues = this->maximum() + abs(this->minimum());
        double newX = ((double)w / (double)totValues) * ((double)value + abs(this->minimum()));
        sliderLabel->setGeometry(newX, -10, 40, 20);
        sliderLabel->setText(QString("%L1").arg((double)value / sliderStep, 0, 'f', 3));
    }
}

void SliderWithTarget::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        double myx = event->x();
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

        //throw aways clicks that are not on the handle
        if (disableClickOutOfHandle)
        {
            if (myx<sr.left()+3 || myx>sr.right()-3)
            {
                return;
            }
        }

        //setValue(minimum() + ((maximum() - minimum()) * myx) / width());
        event->accept();
    }
    QSlider::mousePressEvent(event);
}
