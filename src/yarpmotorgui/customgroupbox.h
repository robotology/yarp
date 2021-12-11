/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CUSTOMGROUPBOX_H
#define CUSTOMGROUPBOX_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>

namespace Ui {
class CustomGroupBox;
}

class CustomGroupBoxLabel : public QPushButton
{
    Q_OBJECT

public:

    explicit CustomGroupBoxLabel(QWidget* parent = nullptr);

    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
signals:

    void sig_titleDoubleClick();
};

class CustomGroupBox : public QWidget
{
    Q_OBJECT

public:
    explicit CustomGroupBox(QWidget *parent = nullptr);
    ~CustomGroupBox();

    void setTitle(const QString& string);

    void setTitleBackgroundColor(const QColor& backgroundColor);

    void removeTitleBackground();

    void setTitleIcon(const QIcon& icon);

    void addWidget(QWidget* widget);

    void toggle(bool visible);

    void enableCollapseAllContextMenu(bool enable);

private slots:
    void onArrowPressed(bool);

    void onTitleDoubleClick();

    void onExpandAll();

    void onCollapseAll();

    void onShowContextMenu(QPoint pos);

private:
    Ui::CustomGroupBox *ui;
    QVBoxLayout* m_layout;
    bool m_visible;
    QMenu m_contextMenu;
};

#endif // CUSTOMGROUPBOX_H
