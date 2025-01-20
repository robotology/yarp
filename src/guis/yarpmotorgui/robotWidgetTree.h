/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


#ifndef ROBOTWIDGETTREE_H
#define ROBOTWIDGETTREE_H

#include <QObject>
#include <QIcon>
#include <QVector>
#include <string>
#include "customgroupbox.h"
#include "partItemTree.h"
#include "jointitem.h"

class RobotWidgetTree : public CustomGroupBox
{
    Q_OBJECT

    struct PartPointers
    {
        PartItemTree* partWidget;
        CustomGroupBox* partWidgetParent;
    };

    std::unordered_map<int, PartPointers> m_indexToPartMap;
    QIcon m_okIcon;
    QIcon m_warningIcon;

public:

    explicit RobotWidgetTree(QWidget *parent = nullptr);

    void setIcons(const QIcon& okIcon, const QIcon& warningIcon);

    void addPart(const std::string &partName, int partIndex, PartItemTree* partTreeWidget);

    void updateRobotPart(int index, const QVector<JointItem::JointState>& modes);

    void resizeEvent(QResizeEvent *event) override;

signals:

    void sig_partDoubleClicked(int index);

private slots:

    void onRobotTitleDoubleClick();

    void onPartTitleDoubleClick(int index);

};

#endif // ROBOTWIDGETTREE_H
