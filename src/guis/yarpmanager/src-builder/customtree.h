/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CUSTOMTREE_H
#define CUSTOMTREE_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <yarp/conf/compiler.h>

class CustomTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTree(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:

public slots:

};

#endif // CUSTOMTREE_H
