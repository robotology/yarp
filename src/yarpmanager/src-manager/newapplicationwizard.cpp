/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "newapplicationwizard.h"
#include <QGridLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <dirent.h>
#include <yarp/conf/filesystem.h>

using namespace std;
using namespace yarp::manager;

inline bool absolute(const char *path) {  //copied from yarp_OS ResourceFinder.cpp
    if (path[0]=='/'||path[0]=='\\') {
        return true;
    }
    std::string str(path);
    if (str.length()>1) {
        if (str[1]==':') {
            return true;
        }
    }
    return false;
}


NewApplicationWizard::NewApplicationWizard(yarp::os::Property *config, bool _saveAs):alreadyExists(false), saveAs(_saveAs)
{
    auto* page = new CustomWizardPage;

    this->m_config = config;
    page->setTitle("Application Properties");
    page->setFinalPage(true);

    nameLbl = new QLabel("Name: ");
    nameLbl->setWordWrap(true);

    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("ApplicationName");

    descrLbl = new QLabel("Description: ");
    descrLbl->setWordWrap(true);

    descrEdit = new QLineEdit();
    descrEdit->setPlaceholderText("Description");

    versionLbl = new QLabel("Version: ");
    versionLbl->setWordWrap(true);
    versionEdit = new QLineEdit();
    versionEdit->setPlaceholderText("1.0");

    authLbl = new QLabel("Authors: ");
    authLbl->setWordWrap(true);

    authEdit = new QLineEdit();

    fileLbl = new QLabel("File: ");
    fileLbl->setWordWrap(true);

    fileEdit = new QLineEdit();
    fileEdit->setReadOnly(true);


    folderLbl = new QLabel("Folder: ");
    folderLbl->setWordWrap(true);

    folderCombo = new QComboBox();
    folderCombo->setEditable(false);
    folderCombo->setDisabled(saveAs);



    browseBtn = new QPushButton("...");
    browseBtn->setDisabled(saveAs);




    auto* layout = new QGridLayout;
    layout->addWidget(nameLbl,0,0);
    layout->addWidget(nameEdit,0,1);

    layout->addWidget(descrLbl,1,0);
    layout->addWidget(descrEdit,1,1);

    layout->addWidget(versionLbl,2,0);
    layout->addWidget(versionEdit,2,1);

    layout->addWidget(authLbl,3,0);
    layout->addWidget(authEdit,3,1);

    layout->addWidget(fileLbl,4,0);
    layout->addWidget(fileEdit,4,1);

    layout->addWidget(folderLbl,5,0);
    layout->addWidget(folderCombo,5,1);
    layout->addWidget(browseBtn,5,2);

    page->setLayout(layout);

    page->registerField("fileName*",fileEdit);
    addPage(page);



    const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};
    if(m_config->check("apppath")){
        string basepath=m_config->check("ymanagerini_dir", yarp::os::Value("")).asString();
        string appPaths(m_config->find("apppath").asString());
        string strPath;

        do
        {
            string::size_type pos=appPaths.find(';');
            strPath=appPaths.substr(0, pos);
            trimString(strPath);
            if (!absolute(strPath.c_str())) {
                strPath = basepath + strPath;
            }

            if ((strPath.rfind(directorySeparator) == string::npos) || (strPath.rfind(directorySeparator) != strPath.size() - 1)) {
                strPath = strPath + string(directorySeparator);
            }
            folderCombo->addItem(QString("%1").arg(strPath.c_str()));

            if (pos == string::npos) {
                break;
            }
            appPaths=appPaths.substr(pos+1);
        }
        while (appPaths!="");
    }
    if (m_config->check("yarpdatahome")){
       string appPaths(m_config->find("apppath").asString());
       string homePath=m_config->find("yarpdatahome").asString();

       homePath +=  string(directorySeparator) + string("applications");

       if (appPaths.find(homePath) == string::npos) {
           folderCombo->addItem(QString("%1").arg(homePath.c_str()));
       }
   }

    if(folderCombo->count() <= 0){
        folderCombo->addItem(QDir::homePath());
    }


    connect(browseBtn,SIGNAL(clicked()),this,SLOT(onBrowse()));
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged(QString)));
    connect(folderCombo, SIGNAL(activated(int)), this, SLOT(onSwitchCall()));

}

void NewApplicationWizard::checkFileAlreadyExists(){
    buildFileName();
    if (fileExists(this->fileName))
    {
        fileEdit->setStyleSheet("color: #FF0000");
        alreadyExists = true;
    }
    else
    {
        fileEdit->setStyleSheet("color: #000000");
        alreadyExists = false;
    }

}


void NewApplicationWizard::onNameChanged(QString name)
{
    if (!name.isEmpty())
    {
        fileEdit->setText(QString("%1.xml").arg(name.toLatin1().data()));
        checkFileAlreadyExists();
    }
    else
    {
        fileEdit->setText("");
    }
}

void NewApplicationWizard::onSwitchCall()
{
    checkFileAlreadyExists();
}

bool NewApplicationWizard::fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists
    if (check_file.exists() && check_file.isFile())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void NewApplicationWizard::buildFileName(){
    QString sep="";
    //checking if the path terminate with / or not
    if (folderCombo->currentText().at(folderCombo->currentText().size()-1) != '/')
    {
        sep = QString{yarp::conf::filesystem::preferred_separator};
    }
    this->fileName = QString("%1"+sep+"%2").arg(folderCombo->currentText().toLatin1().data()).arg(fileEdit->text().toLatin1().data());
}


void NewApplicationWizard::onBrowse( )
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    folderCombo->currentText(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);


    if(!dir.isEmpty()){
        folderCombo->addItem(dir);
        folderCombo->setCurrentText(dir);
        emit folderCombo->activated(folderCombo->count());
    }

}


void NewApplicationWizard::accept()
{
    if (nameEdit->text().isEmpty()){
        name = nameEdit->placeholderText();
    }else{
        name = nameEdit->text();
    }
    if (descrEdit->text().isEmpty()){
        description = descrEdit->placeholderText();
    }else{
        description = descrEdit->text();
    }
    if (versionEdit->text().isEmpty()){
        version = versionEdit->placeholderText();
    }else{
        version = versionEdit->text();
    }
    buildFileName();
    if (alreadyExists)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Quit", "The file chosen already exists, do you want to overwrite it?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
        {
            QDialog::reject();
            return;
        }
    }
    QDialog::accept();
}
