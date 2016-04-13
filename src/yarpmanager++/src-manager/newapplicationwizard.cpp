#include "newapplicationwizard.h"
#include <QGridLayout>
#include <QFileDialog>

#include <yarp/manager/ymm-dir.h>

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


NewApplicationWizard::NewApplicationWizard(yarp::os::Property *config)
{
    CustomWizardPage *page = new CustomWizardPage;

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



    browseBtn = new QPushButton("...");




    QGridLayout *layout = new QGridLayout;
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



    if(m_config->check("apppath")){
        string basepath=m_config->check("ymanagerini_dir", yarp::os::Value("")).asString().c_str();
        string appPaths(m_config->find("apppath").asString().c_str());
        string strPath;

        do
        {
            string::size_type pos=appPaths.find(";");
            strPath=appPaths.substr(0, pos);
            trimString(strPath);
            if (!absolute(strPath.c_str()))
                strPath=basepath+strPath;

            if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
                    (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
                strPath = strPath + string(PATH_SEPERATOR);
            folderCombo->addItem(QString("%1").arg(strPath.c_str()));

            if (pos==string::npos)
                break;
            appPaths=appPaths.substr(pos+1);
        }
        while (appPaths!="");
    }
    if (m_config->check("yarpdatahome")){
       string appPaths(m_config->find("apppath").asString().c_str());
       string homePath=m_config->find("yarpdatahome").asString().c_str();

       homePath +=  string(PATH_SEPERATOR) + string("applications");

       if (appPaths.find(homePath)==string::npos)
           folderCombo->addItem(QString("%1").arg(homePath.c_str()));
   }

    if(folderCombo->count() <= 0){
        folderCombo->addItem(QDir::homePath());
    }


    connect(browseBtn,SIGNAL(clicked()),this,SLOT(onBrowse()));
    connect(nameEdit,SIGNAL(textChanged(QString)),this,SLOT(onNameChanged(QString)));

}

void NewApplicationWizard::onNameChanged(QString name)
{
    if(!name.isEmpty()){
        fileEdit->setText(QString("%1.xml").arg(name.toLatin1().data()));
    }else{
        fileEdit->setText("");
    }
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
    }

}


void NewApplicationWizard::accept()
{
    if(nameEdit->text().isEmpty()){
        name = nameEdit->placeholderText();
    }else{
        name = nameEdit->text();
    }
    if(descrEdit->text().isEmpty()){
        description = descrEdit->placeholderText();
    }else{
        description = descrEdit->text();
    }
    if(versionEdit->text().isEmpty()){
        version = versionEdit->placeholderText();
    }else{
        version = versionEdit->text();
    }

    this->fileName = QString("%1/%2").arg(folderCombo->currentText().toLatin1().data()).arg(fileEdit->text().toLatin1().data());
    QDialog::accept();
}


