/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SLIDER_WITH_TARGET_H
#define SLIDER_WITH_TARGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QStyleOptionSlider>
#include <yarp/conf/compiler.h>

class SliderWithTarget : public QSlider
{
public:
    bool enableViewLabel;
    bool enableViewTarget;
    bool disableClickOutOfHandle;
    SliderWithTarget(QWidget * parent = 0, bool _hasTargetOption = true);
    ~SliderWithTarget() { if (sliderLabel) { delete sliderLabel; sliderLabel = 0; } }
    void updateSliderTarget(double val);
    void resetTarget();
    void setSliderStep(double val);
    double getSliderStep();
    void setValue(double val);
    void setIsDouble(bool b);
    bool getIsDouble();

protected:
    bool isDouble;
    QLabel* sliderLabel;
    double target;
    double width_at_target;
    double sliderStep;
    bool hasTargetOption;
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent * event) override;
};


#endif // JOINTITEM_H
