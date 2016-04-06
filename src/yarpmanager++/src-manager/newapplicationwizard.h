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
    NewApplicationWizard(yarp::os::Property *);

public:
    QString name;
    QString description;
    QString version;
    QString authors;
    QString fileName;

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



signals:
    void wizardError(QString);

private slots:
    void onBrowse();
    void onNameChanged(QString name);

public slots:
    void accept();
};

class CustomWizardPage : public QWizardPage
{
    Q_OBJECT
    friend class NewApplicationWizard;

public:
    CustomWizardPage(QWidget * parent = 0) : QWizardPage(parent){}


};

#endif // NEWAPPLICATIONWIZARD_H
