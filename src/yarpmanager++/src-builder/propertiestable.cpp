#include "propertiestable.h"
#include <QDebug>

PropertiesTable::PropertiesTable(Manager *manager,QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    propertiesTab = new QTabWidget(this);
    appProperties = new QTreeWidget(this);
    moduleProperties = new QTreeWidget(this);
    moduleDescription = new QTreeWidget(this);
    currentApplication = NULL;
    currentModule = NULL;
    this->manager = manager;

    paramsSignalMapper = new QSignalMapper(this);
    connect(paramsSignalMapper, SIGNAL(mapped(QWidget*)),
                this, SLOT(onComboChanged(QWidget*)));

    appProperties->setHeaderLabels(QStringList() << "Property" << "Value");
    moduleProperties->setHeaderLabels(QStringList() << "Property" << "Value");
    moduleDescription->setHeaderLabels(QStringList() << "Item" << "Value");
    lay->addWidget(propertiesTab);
    setLayout(lay);
    lay->setMargin(0);

    appName = NULL;
    appDescr = NULL;
    appPrefix = NULL;
    appVersion = NULL;
    appAuthors = NULL;
    nodeCombo = NULL;
    deployerCombo = NULL;

    propertiesTab->addTab(appProperties,"Application Properties");

}

void PropertiesTable::showApplicationTab(Application *application)
{
    if(!propertiesTab){
        return;
    }
    currentModule = NULL;
    currentApplication = application;
    disconnect(appProperties,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onAppItemChanged(QTreeWidgetItem*,int)));
    disconnect(appProperties,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onAppItemDoubleClicked(QTreeWidgetItem*,int)));
    appProperties->show();
    moduleProperties->hide();
    moduleDescription->hide();

    propertiesTab->clear();
    propertiesTab->addTab(appProperties,"Application Properties");
    appProperties->clear();
    if(appProperties->topLevelItemCount() <=0){
        appName = new QTreeWidgetItem(appProperties,QStringList() << "Name" << application->getName());
        appDescr = new QTreeWidgetItem(appProperties,QStringList() << "Description" << application->getDescription());
        appVersion = new QTreeWidgetItem(appProperties,QStringList() << "Version" << application->getVersion());
        appPrefix = new QTreeWidgetItem(appProperties,QStringList() << "Prefix" << application->getPrefix());

        QString authors;
        for(int i=0;i<application->authorCount();i++){
            authors = QString("%1%2;").arg(authors).arg(application->getAuthorAt(i).getName());
        }
        appAuthors = new QTreeWidgetItem(appProperties,QStringList() << "Authors" << authors);

        appProperties->addTopLevelItem(appName);
        appProperties->addTopLevelItem(appDescr);
        appProperties->addTopLevelItem(appPrefix);
        appProperties->addTopLevelItem(appVersion);
        appProperties->addTopLevelItem(appAuthors);

    }
    connect(appProperties,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onAppItemChanged(QTreeWidgetItem*,int)));
    connect(appProperties,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onAppItemDoubleClicked(QTreeWidgetItem*,int)));
}


void PropertiesTable::showModuleTab(Module *mod)
{
    modules.clear();
    disconnect(moduleProperties,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onModItemChanged(QTreeWidgetItem*,int)));
    disconnect(moduleProperties,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onModItemDoubleClicked(QTreeWidgetItem*,int)));

    currentApplication = NULL;
    currentModule = mod;
    propertiesTab->clear();
    propertiesTab->addTab(moduleProperties,"Module Properties");
    propertiesTab->addTab(moduleDescription,"Description");
    moduleProperties->clear();
    moduleDescription->clear();

    appProperties->hide();
    moduleProperties->show();
    moduleDescription->show();

    modName = new QTreeWidgetItem(moduleProperties,QStringList() << "Name" << mod->getName());
    modNode = new QTreeWidgetItem(moduleProperties,QStringList() << "Node");
    modStdio = new QTreeWidgetItem(moduleProperties,QStringList() << "Stdio" << mod->getStdio());
    modWorkDir = new QTreeWidgetItem(moduleProperties,QStringList() << "Workdir" << mod->getWorkDir());
    modPrefix = new QTreeWidgetItem(moduleProperties,QStringList() << "Prefix" << mod->getBasePrefix());
    modDeployer = new QTreeWidgetItem(moduleProperties,QStringList() << "Deployer");
    modParams = new QTreeWidgetItem(moduleProperties,QStringList() << "Parameters" << mod->getParam());

    lastPrefix = mod->getBasePrefix();

    modStdio->setFlags(modStdio->flags() | Qt::ItemIsEditable);
    modWorkDir->setFlags(modWorkDir->flags() | Qt::ItemIsEditable);
    modPrefix->setFlags(modPrefix->flags() | Qt::ItemIsEditable);

    moduleProperties->addTopLevelItem(modName);
    moduleProperties->addTopLevelItem(modNode);
    moduleProperties->addTopLevelItem(modStdio);
    moduleProperties->addTopLevelItem(modWorkDir);
    moduleProperties->addTopLevelItem(modPrefix);
    moduleProperties->addTopLevelItem(modDeployer);
    moduleProperties->addTopLevelItem(modParams);

    if(deployerCombo){
        delete deployerCombo;
        deployerCombo = NULL;
    }

    if(nodeCombo){
        delete nodeCombo;
        nodeCombo = NULL;
    }
    deployerCombo = new QComboBox();
    nodeCombo = new QComboBox();

    deployerCombo->setEditable(true);
    nodeCombo->setEditable(true);

    if(compareString(mod->getBroker(),"yarpdev")){
        deployerCombo->addItem("yarpdev");
    }else if(compareString(mod->getBroker(),"icubmoddev")){
        deployerCombo->addItem("icubmoddev");
    }else{
        deployerCombo->addItem("local");
        deployerCombo->addItem("yarprun");
    }

    if(strlen(mod->getBroker())){
        deployerCombo->setCurrentText(mod->getBroker());
    }else if(compareString(mod->getHost(),"localhost")){
        deployerCombo->setCurrentText("local");
    }
    if(mod->getNeedDeployer()){
        deployerCombo->setEditable(false);
    }


    nodeCombo->addItem(mod->getHost());
    if(QString(mod->getHost()) != "localhost"){
        nodeCombo->addItem("localhost");
    }
    ResourcePContainer resources = manager->getKnowledgeBase()->getResources();
    for(ResourcePIterator itr=resources.begin(); itr!=resources.end(); itr++){
        Computer* comp = dynamic_cast<Computer*>(*itr);
        if(comp && !compareString(comp->getName(), "localhost")){
            nodeCombo->addItem(comp->getName());
        }
    }
    connect(nodeCombo, SIGNAL(editTextChanged(QString)), paramsSignalMapper, SLOT(map()));
    connect(nodeCombo, SIGNAL(currentIndexChanged(int)), paramsSignalMapper, SLOT(map()));
    paramsSignalMapper->setMapping(nodeCombo,nodeCombo);

    connect(deployerCombo, SIGNAL(editTextChanged(QString)), paramsSignalMapper, SLOT(map()));
    connect(deployerCombo, SIGNAL(currentIndexChanged(int)), paramsSignalMapper, SLOT(map()));
    paramsSignalMapper->setMapping(deployerCombo,deployerCombo);




     /*****************************/
     // Populate paramters
     for(int i=0;i<mod->argumentCount();i++){
         Argument a = mod->getArgumentAt(i);
         QTreeWidgetItem *it = new QTreeWidgetItem(modParams,QStringList() << a.getParam());
         QComboBox *paramCombo = new QComboBox();
         paramCombo->setEditable(true);
         paramCombo->addItem(a.getValue());
         if(strcmp(a.getDefault(),a.getValue()) != 0 ){
            paramCombo->addItem(a.getDefault());
         }
         moduleProperties->setItemWidget(it,1,paramCombo);
         connect(paramCombo, SIGNAL(editTextChanged(QString)), paramsSignalMapper, SLOT(map()));
         connect(paramCombo, SIGNAL(currentIndexChanged(int)), paramsSignalMapper, SLOT(map()));
         paramsSignalMapper->setMapping(paramCombo,paramCombo);
     }
     /*****************************/

    moduleProperties->setItemWidget(modDeployer,1,deployerCombo);
    moduleProperties->setItemWidget(modNode,1,nodeCombo);
    modParams->setExpanded(true);


    QTreeWidgetItem *nameItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Name" << mod->getName());
    QTreeWidgetItem *versionItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Version" << mod->getVersion());
    QTreeWidgetItem *descriptionItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Description" << mod->getDescription());
    QTreeWidgetItem *parametersItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Parameters");
    for(int i=0;i<mod->argumentCount();i++){
        Argument a = mod->getArgumentAt(i);
        QTreeWidgetItem *it = new QTreeWidgetItem(parametersItem,QStringList() << a.getParam() << a.getDescription());

    }

    QTreeWidgetItem *authorsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Authors" );
    for(int i=0;i<mod->authorCount();i++){
        Author a = mod->getAuthorAt(i);
        QTreeWidgetItem *it = new QTreeWidgetItem(authorsItem,QStringList() << a.getName() << a.getEmail());
    }

    QTreeWidgetItem *inputsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Inputs" );
    for(int i=0;i<mod->inputCount();i++){
        InputData a = mod->getInputAt(i);

        QTreeWidgetItem *type = new QTreeWidgetItem(inputsItem,QStringList() << "Type" << a.getName());
        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
        QTreeWidgetItem *req = new QTreeWidgetItem(type,QStringList() << "Required" << (a.isRequired() ? "yes" : "no"));
        Q_UNUSED(port);
        Q_UNUSED(desc);
        Q_UNUSED(req);
    }

    QTreeWidgetItem *outputsItem = new QTreeWidgetItem(moduleDescription,QStringList() << "Outputs" );
    for(int i=0;i<mod->outputCount();i++){
        OutputData a = mod->getOutputAt(i); //TODO controllare

        QTreeWidgetItem *type = new QTreeWidgetItem(outputsItem,QStringList() << "Type" << a.getName());
        QTreeWidgetItem *port = new QTreeWidgetItem(type,QStringList() << "Port" << a.getPort());
        QTreeWidgetItem *desc = new QTreeWidgetItem(type,QStringList() << "Description" << a.getDescription());
        Q_UNUSED(port);
        Q_UNUSED(desc);
    }

    moduleDescription->addTopLevelItem(nameItem);
    moduleDescription->addTopLevelItem(versionItem);
    moduleDescription->addTopLevelItem(descriptionItem);
    moduleDescription->addTopLevelItem(parametersItem);
    moduleDescription->addTopLevelItem(authorsItem);
    moduleDescription->addTopLevelItem(inputsItem);
    moduleDescription->addTopLevelItem(outputsItem);

    connect(moduleProperties,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onModItemChanged(QTreeWidgetItem*,int)));
    connect(moduleProperties,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onModItemDoubleClicked(QTreeWidgetItem*,int)));

}

void PropertiesTable::onAppItemChanged(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if (!(tmp & Qt::ItemIsEditable)) {
        return;
    }

    if(currentApplication){
        currentApplication->setDescription(appDescr->text(1).toLatin1().data());
        currentApplication->setVersion(appVersion->text(1).toLatin1().data());
        currentApplication->setPrefix(appPrefix->text(1).toLatin1().data());
        //manager->getKnowledgeBase()->setApplicationPrefix(currentApplication, appPrefix->text(1).toLatin1().data(), false);
    }



    modified();
}

/*! \brief Called when an item has been double clicked */
void PropertiesTable::onAppItemDoubleClicked(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if (appProperties->indexOfTopLevelItem(it) > 0 &&  col == 1) {
        it->setFlags(tmp | Qt::ItemIsEditable);
    } else if (tmp & Qt::ItemIsEditable) {
        it->setFlags(tmp ^ Qt::ItemIsEditable);
    }
}

void PropertiesTable::onModItemChanged(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if (!(tmp & Qt::ItemIsEditable)) {
        return;
    }


    if(currentModule){
        currentModule->setStdio(modStdio->text(1).toLatin1().data());
        currentModule->setWorkDir(modWorkDir->text(1).toLatin1().data());
        if(lastPrefix != modPrefix->text(1)){
            currentModule->setBasePrefix(modPrefix->text(1).toLatin1().data());

            string strPrefix;
            Application* application = manager->getKnowledgeBase()->getApplication();
            if(application){
                strPrefix = string(application->getPrefix()) + string(modPrefix->text(1).toLatin1().data());
                for(int j=0; j<currentModule->outputCount(); j++){
                    OutputData *output = &currentModule->getOutputAt(j);

                    for(int i=0; i<application->connectionCount(); i++){
                        Connection con = application->getConnectionAt(i);
                        //string strOldFrom = con.from();
                        Connection updatedCon = con;
                        if(con.getCorOutputData()){
                            if(con.getCorOutputData() == output){
                                string strFrom = strPrefix + string(output->getPort());
                                updatedCon.setFrom(strFrom.c_str());
                                manager->getKnowledgeBase()->updateConnectionOfApplication(application,
                                                            con, updatedCon);
                            }
                        }
                    }
                }
            }

            for(int j=0; j<currentModule->inputCount(); j++){
                InputData *input = &currentModule->getInputAt(j);
                for(int i=0; i<application->connectionCount(); i++){
                    Connection con = application->getConnectionAt(i);
                    Connection updatedCon = con;
                    if(con.getCorInputData()){
                        if(con.getCorInputData() == input){
                            string strTo = strPrefix + string(input->getPort());
                            updatedCon.setTo(strTo.c_str());
                            manager->getKnowledgeBase()->updateConnectionOfApplication(application,
                                                        con, updatedCon);
                        }
                    }
                }
            }
            manager->getKnowledgeBase()->setModulePrefix(currentModule, strPrefix.c_str(), false);
        }

    }

    for(int i=0;i<modules.count();i++){
        Module *module = modules.at(i);
        if(!strcmp(module->getName(),currentModule->getName())){
             module->setStdio(currentModule->getStdio());
             module->setWorkDir(currentModule->getWorkDir());
        }
    }
    modified();
}

/*! \brief Called when an item has been double clicked */
void PropertiesTable::onModItemDoubleClicked(QTreeWidgetItem *it,int col)
{
    Qt::ItemFlags tmp = it->flags();
    if ((moduleProperties->indexOfTopLevelItem(it) == 2 || moduleProperties->indexOfTopLevelItem(it) == 3 ||
         moduleProperties->indexOfTopLevelItem(it) == 4)  &&  col == 1) {
        it->setFlags(tmp | Qt::ItemIsEditable);
    } else if (tmp & Qt::ItemIsEditable) {
        it->setFlags(tmp ^ Qt::ItemIsEditable);
    }
}

void PropertiesTable::onComboChanged(QWidget *combo)
{
    if(combo == deployerCombo){
        if(currentModule){
            currentModule->setBroker(((QComboBox*)combo)->currentText().toLatin1().data());
        }
        modified();
        return;
    }
    if(combo == nodeCombo){

        if(currentModule){
            currentModule->setHost(((QComboBox*)combo)->currentText().toLatin1().data());
        }
        modified();
        return;
    }

    QString params;
    for(int i=0; i<modParams->childCount();i++){
        QTreeWidgetItem *c = modParams->child(i);
        QComboBox *combo = ((QComboBox*)moduleProperties->itemWidget(c,1));
        if(!combo->currentText().isEmpty()){
            params = params + QString("--%1 %2 ").arg(c->text(0).toLatin1().data())
                    .arg(combo->currentText());
        }
    }
    modParams->setText(1,params);
    if(currentModule){
        currentModule->setParam(params.toLatin1().data());
    }

    for(int i=0;i<modules.count();i++){
        Module *module = modules.at(i);
        if(!strcmp(module->getName(),currentModule->getName())){
             module->setParam(currentModule->getParam());
             module->setHost(currentModule->getHost());
             module->setBroker(currentModule->getBroker());
        }
    }
    modified();
}

void PropertiesTable::addModules(Module *mod)
{
    modules.append(mod);
}
