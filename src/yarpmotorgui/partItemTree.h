/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef PARTITEMTREE_H
#define PARTITEMTREE_H

#include <QWidget>
#include <unordered_map>


#include "jointItemTree.h"
#include "flowlayout.h"

class PartItemTree : public QWidget
{
    Q_OBJECT
    FlowLayout *m_layout;

public:
    explicit PartItemTree(QWidget *parent = nullptr);

    int numberOfJoints() const;

    JointItemTree* addJoint();

    JointItemTree* getJoint(int index);

    virtual QSize sizeHint() const override;
};

#endif // PARTITEMTREE_H
