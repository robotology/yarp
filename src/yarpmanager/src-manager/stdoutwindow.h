/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STDOUTWINDOW_H
#define STDOUTWINDOW_H

#include <QWidget>
#include <yarp/conf/compiler.h>

namespace Ui {
class StdoutWindow;
}

class StdoutWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StdoutWindow(int id, QString title, QWidget *parent = 0);
    ~StdoutWindow();

    int getId();
    void addMessage(QString text);

protected:
    void closeEvent(QCloseEvent * event) override;

private:
    Ui::StdoutWindow *ui;
    int id;



signals:
    void closeStdOut(int);
    void internalAddMessage(QString msg);

public slots:
    void onInternalAddMessage(QString msg);
};

#endif // STDOUTWINDOW_H
