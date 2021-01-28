/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#ifndef NEWAPPLICATIONWIZARD_H
#define NEWAPPLICATIONWIZARD_H

#include <QWidget>
#include <QWizard>
#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include <yarp/os/Property.h>
#include <yarp/manager/application.h>

class NewApplicationWizard : public QWizard
{
    Q_OBJECT
public:
    NewApplicationWizard(yarp::os::Property * config, bool _saveAs=false);

public:
    QString name;
    QString description;
    QString version;
    QString authors;
    QString fileName;
    bool alreadyExists;

private:
    QLabel *nameLbl;
    QLineEdit *nameEdit;
    QLabel *descrLbl;
    QLineEdit *descrEdit;
    QLabel *versionLbl;
    QLineEdit *versionEdit;
    QLabel *authLbl;
    QLineEdit *authEdit;
    QLabel *folderLbl;
    QComboBox *folderCombo;
    QPushButton *browseBtn;
    QLabel *fileLbl;
    QLineEdit *fileEdit;

    yarp::os::Property *m_config;
    bool saveAs;



signals:
    void wizardError(QString);

private slots:
    void onBrowse();
    void checkFileAlreadyExists();
    void onNameChanged(QString name);
    void onSwitchCall();
    bool fileExists(QString path);
    void buildFileName();

public slots:
    void accept() override;
};

class CustomWizardPage : public QWizardPage
{
    Q_OBJECT
    friend class NewApplicationWizard;

public:
    CustomWizardPage(QWidget * parent = 0) : QWizardPage(parent){}


};

#endif // NEWAPPLICATIONWIZARD_H
