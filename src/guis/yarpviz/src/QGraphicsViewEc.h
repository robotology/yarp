/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef QGRAPHICSVIEWEC_H
#define QGRAPHICSVIEWEC_H

#include <QGraphicsView>

/**
 * @brief Graphics view with wheel zoom
 *
 */
class QGraphicsViewEc : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsViewEc(QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // QGRAPHICSVIEWEC_H
