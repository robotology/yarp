/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Ali Paikan and Vadim Tikhanoff
 * email:  ali.paikan@iit.it vadim.tikhanoff@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <signal.h>
#include <gtkmm.h>
#include <iostream>
#include <string>
#include "iCub/main_window.h"

#define WND_DEF_HEIGHT          400
#define WND_DEF_WIDTH           800
#define CMD_HELP                VOCAB4('h','e','l','p')
#define CMD_STEP                VOCAB4('s','t','e','p')
#define CMD_QUIT                VOCAB4('q','u','i','t')
#define CMD_SET                 VOCAB3('s','e','t')
#define CMD_GET                 VOCAB3('g','e','t')
#define CMD_LOAD                VOCAB4('l','o','a','d')
#define CMD_PLAY                VOCAB4('p','l','a','y')
#define CMD_STOP                VOCAB4('s','t','o','p')
#define CMD_PAUSE               VOCAB4('p','a','u','s')

using namespace std;
using namespace yarp::os;

/**********************************************************/
void sighandler(int sig)
{
    fprintf(stdout, "\n\nCaught ctrl-c, please quit within gui for clean exit\n\n");
}
/**********************************************************/
MainWindow::MainWindow(yarp::os::ResourceFinder    &rf)
{
    moduleName =  rf.check("name", Value("dataSetPlayer"), "module name (string)").asString();

    if (rf.check("withExtraTimeCol"))
    {
        withExtraTimeCol = true;
        column = rf.check("withExtraTimeCol",Value(1)).asInt();

        if (column < 1 || column > 2 )
            column = 1;

        fprintf(stdout, "Selected timestamp column to check is %d \n", column);

    }else
    {
        withExtraTimeCol = false;
        column = 0;
    }


    add_prefix= rf.check("add_prefix");
    createUtilities();
    set_title( (const Glib::ustring) moduleName );
    set_default_size(WND_DEF_WIDTH, WND_DEF_HEIGHT);
    subDirCnt = 0;
    setupActions();
    createWidgets();
    setupSignals();
    show_all_children();
    //initialize();
    ::signal(SIGINT, sighandler);
    ::signal(SIGTERM, sighandler);
    rpcPort.open( ("/"+moduleName+"/rpc:i").c_str() );
    attach(rpcPort);
}

/************************************************************************/
bool MainWindow::attach(RpcServer &source)
{
    return this->yarp().attachAsServer(source);
}

/************************************************************************/
bool MainWindow::step()
{
    Bottle reply;
    stepFromCommand(reply);
    if (reply.toString() == "error")
        return false;
    if (reply.toString() == "ok")
        return true;
    return false;
}

/************************************************************************/
bool MainWindow::setFrame(const string &name, const int frameNum)
{
    updateFrameNumber(name.c_str(), frameNum);
    return true;
}

/************************************************************************/
int MainWindow::getFrame(const string &name)
{
    int frame = getFrameCmd(name.c_str());
    if (frame < 1)
        return -1;
    else
        return frame;
}

/************************************************************************/
bool MainWindow::load(const string &path)
{
    string cmdPath = path.c_str();
    for (int x=0; x < subDirCnt; x++)
        utilities->closePorts(utilities->partDetails[x]);

    clearUtilities();
    createUtilities();
    subDirCnt = 0;
    size_t slashErr = cmdPath.find('/');

    if (slashErr!=string::npos)
        doGuiSetup(cmdPath);
    else
    {
        fprintf(stderr,"Error, please make sure you are using forward slashes '/' in path.\n");
    }

    if (subDirCnt > 0 )
        return true;
    else
        return false;
    return true;
}

/**********************************************************/
bool MainWindow::play()
{
    onMenuPlayBackPlay();
    return true;
}

/**********************************************************/
bool MainWindow::pause()
{
    onMenuPlayBackPause();
    return true;
}

/**********************************************************/
bool MainWindow::stop()
{
    onMenuPlayBackStop();
    return true;
}

/**********************************************************/
bool MainWindow::quit()
{
    onCommandQuit();
    return true;
}

/**********************************************************/
MainWindow::~MainWindow()
{
    fprintf(stdout,"cleaning up rpc port...\n");
    rpcPort.close();
    fprintf(stdout,"done cleaning rpc port...\n");
}
/**********************************************************/
int MainWindow::initialize(void)
{
    //load files and add parts to the list at init
    string message = utilities->getCurrentPath();    //Get current directory path
    doGuiSetup(message);                            //find all subdir and files
    return 0;
}
/**********************************************************/
void MainWindow::createUtilities()
{
    utilities = new Utilities(moduleName,add_prefix);
    utilities->withExtraColumn = withExtraTimeCol;
    utilities->column = column;
}
/**********************************************************/
void MainWindow::clearUtilities()
{
    delete utilities;
}
/**********************************************************/
int MainWindow::doGuiSetup(string newPath)
{
    m_Statusbar.push(newPath.c_str());//Write path to the gui
    //look for folders and log files associated with them
    fprintf(stdout,"the full path is %s \n", newPath.c_str());
    subDirCnt = 0;
    partsName.clear();
    partsFullPath.clear();
    partsInfoPath.clear();
    partsLogPath.clear();
    
    utilities->resetMaxTimeStamp();

    subDirCnt = utilities->getRecSubDirList(newPath.c_str(), partsName, partsInfoPath, partsLogPath, partsFullPath, 1);
    fprintf(stdout,"the size of subDirs is: %d\n", subDirCnt);

    //reset totalSent to 0
    utilities->totalSent = 0;
    utilities->totalThreads = subDirCnt;

    if (subDirCnt > 0)
        utilities->partDetails = new partsData [subDirCnt];//resize(subDirCnt);

    //fill in parts with all data
    for (int x=0; x < subDirCnt; x++)
    {
        utilities->partDetails[x].name = partsName[x];
        utilities->partDetails[x].infoFile = partsInfoPath[x];
        utilities->partDetails[x].logFile = partsLogPath[x];
        utilities->partDetails[x].path = partsFullPath[x];
        utilities->setupDataFromParts(utilities->partDetails[x]);

        utilities->partDetails[x].worker = new WorkerClass(x, subDirCnt);
        utilities->partDetails[x].worker->setManager(utilities);
    }

    //get the max timestamp of all the parts for synchronization
    if (subDirCnt > 0)
        utilities->getMaxTimeStamp();

    //set initial frames for all parts depending on first timestamps
    for (int x=0; x < subDirCnt; x++)
        utilities->initialFrame.push_back( utilities->partDetails[x].currFrame) ;

    utilities->masterThread = new MasterThread(utilities, subDirCnt, this);
    utilities->masterThread->stepfromCmd = false;
    itr = 0;
    partMap.clear();
    //add the parts to the gui
    for (int x=0; x < subDirCnt; x++)
    {
        addPart(utilities->partDetails[x].name.c_str(), utilities->partDetails[x].type.c_str(), utilities->partDetails[x].maxFrame, utilities->partDetails[x].portName.c_str() );
        setInitialPartProgress(utilities->partDetails[x].name.c_str(), 0);
        utilities->configurePorts(utilities->partDetails[x]);
    }
    return 0;
}
/**********************************************************/
void MainWindow::addPart(const char* szName, const char* type, int frames, const char* portName, const char* szFileName )
{
    partMap[szName] = itr;
    m_partRow = *(m_refTreeModel->append());
    m_partRow[m_partColumns.m_col_enabled] = true;
    if(szName)
        m_partRow[m_partColumns.m_col_part] = Glib::ustring(szName);
    if (type)
        m_partRow[m_partColumns.m_col_type] = Glib::ustring(type);

    m_partRow[m_partColumns.m_col_frames] = frames;
    m_partRow[m_partColumns.m_col_percentage] = 0;
    if(szFileName)
        m_partRow[m_partColumns.m_col_filename] = Glib::ustring(szFileName);
    if(portName)
        m_partRow[m_partColumns.m_col_port] = Glib::ustring(portName);
    itr++;
}
/**********************************************************/
bool MainWindow::updateFrameNumber(const char* part, int frameNum)
{
    if (subDirCnt > 0)
    {
        fprintf(stdout,"setting initial frame to %d\n",frameNum);
        //if (frameNum == 0)
            //frameNum = 1;

        for (std::map<const char*,int>::iterator itr=partMap.begin(); itr != partMap.end(); itr++)
        {
            utilities->masterThread->virtualTime = utilities->partDetails[(*itr).second].timestamp[utilities->partDetails[(*itr).second].currFrame];
            utilities->partDetails[(*itr).second].currFrame = frameNum;
        }
        utilities->masterThread->virtualTime = utilities->partDetails[0].timestamp[utilities->partDetails[0].currFrame];
        return true;
    }
    else
        return false;
}
/**********************************************************/
int MainWindow::getFrameCmd( const char* part )
{
    int frame = 0;
    if (subDirCnt > 0)
    {
        for (std::map<const char*,int>::iterator itr=partMap.begin(); itr != partMap.end(); itr++)
            if (strcmp (part,(*itr).first) == 0)
                frame = utilities->partDetails[(*itr).second].currFrame;
    }
    return frame;
}
/**********************************************************/
void MainWindow::stepFromCommand(Bottle &reply)
{
    if (subDirCnt > 0)
    {
        utilities->stepThread();
        reply.addString("ok");
    }
    else
        reply.addString("error");
}
/**********************************************************/
void MainWindow::createWidgets(void)
{
    add(m_VBox);

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    add_accel_group(m_refUIManager->get_accel_group());

    Glib::ustring ui_info =
        "<ui>"
        " <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menuitem action='FileOpen'/>"
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='PlayBackMenu'>"
        "      <menuitem action='PlayBackPlay'/>"
        "      <menuitem action='PlayBackPause'/>"
        "      <menuitem action='PlayBackStop'/>"
        "      <menuitem action='PlayBackForward'/>"
        "      <menuitem action='PlayBackBackward'/>"
        "    </menu>"
        "    <menu action='OptionMenu'>"
        "      <menuitem action='SpeedUp'/>"
        "      <menuitem action='SpeedDown'/>"
        "      <menuitem action='SpeedNormal'/>"
        "      <separator/>"
        "      <menuitem action='PlayBackRepeat'/>"
        "      <menuitem action='PlayBackStrict'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        " </menubar>"
        "</ui>";


#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try
    {
        m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex)
    {
        std::cerr << "building menus failed: " << ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get())
    {
        std::cerr << "building menus failed: " << ex->what();
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED

    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
    if(pMenubar)
        m_VBox.pack_start(*pMenubar, Gtk::PACK_SHRINK);

    // creating tree view
    m_ScrollView.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_ScrollView.add(m_TreeView);
    m_VBox.pack_start(m_ScrollView);

    //creating playback slider
    m_hPlayScale.set_update_policy(Gtk::UPDATE_CONTINUOUS);
    m_hPlayScale.set_range(0.0,100.0);
    m_hPlayScale.set_value(0.0);
    //m_hPlayScale.set_digits(0);
    //m_hPlayScale.set_value_pos(Gtk::POS_RIGHT);
    m_hPlayScale.set_draw_value(false);
    m_hPlayScale.set_size_request(-1, 25);
    m_VBox.pack_start(m_hPlayScale, Gtk::PACK_SHRINK);

    //create tool buttons
    m_playButton.set_stock_id(Gtk::Stock::MEDIA_PLAY);
    m_stopButton.set_stock_id(Gtk::Stock::MEDIA_STOP);
    m_forwardButton.set_stock_id(Gtk::Stock::MEDIA_FORWARD);
    m_rewindButton.set_stock_id(Gtk::Stock::MEDIA_REWIND);
    m_HBox.pack_start(m_rewindButton, Gtk::PACK_SHRINK);
    m_HBox.pack_start(m_playButton, Gtk::PACK_SHRINK);
    m_HBox.pack_start(m_stopButton, Gtk::PACK_SHRINK);
    m_HBox.pack_start(m_forwardButton, Gtk::PACK_SHRINK);

    //adding speed slider
    m_labScale.set_text(" Speed: ");
    m_HBox.pack_start(m_labScale, Gtk::PACK_SHRINK);
    m_hScale.set_update_policy(Gtk::UPDATE_CONTINUOUS);
    m_hScale.set_range(0.0,2.0);
    m_hScale.set_value(1.0);
    m_hScale.set_digits(0);
    m_hScale.set_draw_value(false);
    m_hScale.set_size_request(100, 25);
    m_HBox.pack_start(m_hScale, Gtk::PACK_SHRINK);
    m_labScaleValue.set_text("1.0x");
    m_HBox.pack_start(m_labScaleValue, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_HBox, Gtk::PACK_SHRINK);

    // adding status bar
    m_VBox.pack_start(m_Statusbar, Gtk::PACK_SHRINK);

    // preparing view list
    m_refTreeModel = Gtk::ListStore::create(m_partColumns);
    m_TreeView.set_model(m_refTreeModel);
    m_TreeView.get_selection()->set_mode(Gtk::SELECTION_EXTENDED);

    m_TreeView.append_column_editable("Active", m_partColumns.m_col_enabled);
    m_TreeView.append_column("Part", m_partColumns.m_col_part);
    m_TreeView.append_column("Type", m_partColumns.m_col_type);
    m_TreeView.append_column("Frames", m_partColumns.m_col_frames);
    m_TreeView.append_column("Sample Rate", m_partColumns.m_col_frameRate);
    m_TreeView.append_column_editable("Port Name" , m_partColumns.m_col_port);

    Gtk::CellRendererProgress* cell = Gtk::manage(new Gtk::CellRendererProgress);
    int cols_count = m_TreeView.append_column("Status", *cell);
    Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);
    if(pColumn)
    {
#ifdef GLIBMM_PROPERTIES_ENABLED
        pColumn->add_attribute(cell->property_value(), m_partColumns.m_col_percentage);
#else
        pColumn->add_attribute(*cell, "value", m_partColumns.m_col_percentage);
#endif
    }

    m_TreeView.get_column(0)->set_resizable(true);
    m_TreeView.get_column(0)->set_alignment(Gtk::ALIGN_CENTER);
    m_TreeView.get_column(1)->set_resizable(true);
    m_TreeView.get_column(1)->set_alignment(Gtk::ALIGN_CENTER);
    m_TreeView.get_column(2)->set_resizable(true);
    m_TreeView.get_column(2)->set_alignment(Gtk::ALIGN_CENTER);
    m_TreeView.get_column(3)->set_resizable(true);
    m_TreeView.get_column(3)->set_alignment(Gtk::ALIGN_CENTER);
    m_TreeView.get_column(4)->set_resizable(true);
    m_TreeView.get_column(4)->set_alignment(Gtk::ALIGN_CENTER);
    m_TreeView.get_column(5)->set_resizable(true);
    m_TreeView.get_column(5)->set_alignment(Gtk::ALIGN_CENTER);
    m_TreeView.get_column(6)->set_resizable(true);
    m_TreeView.get_column(6)->set_alignment(Gtk::ALIGN_CENTER);
}

/**********************************************************/
void MainWindow::setupSignals(void)
{
    //Connect signal:
    m_playButton.signal_clicked().connect(sigc::mem_fun(*this,
            &MainWindow::onPlayButtonTogle));
    m_stopButton.signal_clicked().connect(sigc::mem_fun(*this,
            &MainWindow::onMenuPlayBackStop));
    m_forwardButton.signal_clicked().connect(sigc::mem_fun(*this,
            &MainWindow::onMenuPlayBackForward));
    m_rewindButton.signal_clicked().connect(sigc::mem_fun(*this,
            &MainWindow::onMenuPlayBackBackward));
    m_hScale.signal_value_changed().connect(sigc::mem_fun(*this,
            &MainWindow::onSpeedValueChanged));
    m_hPlayScale.signal_value_changed().connect(sigc::mem_fun(*this,
            &MainWindow::onPlayPositionChanged));

    m_hPlayScale.signal_move_slider().connect(sigc::mem_fun(*this,
            &MainWindow::onPlayPositionSlided));
    //m_refTreeModel->signal_row_changed().connect(sigc::mem_fun(*this,
            //&MainWindow::onTreeRowChanged));
    signal_delete_event().connect(sigc::mem_fun(*this,
            &MainWindow::onDeleteEvent));
}

/**********************************************************/
void MainWindow::setupActions(void)
{
    //Create actions for menus and toolbars:
    m_refActionGroup = Gtk::ActionGroup::create();

    //File menu:
    m_refActionGroup->add(Gtk::Action::create("FileMenu", "File"));
    m_refActionGroup->add( Gtk::Action::create("FileOpen", Gtk::Stock::OPEN  ,"_Open directory", "Open directory"),
                        sigc::mem_fun(*this, &MainWindow::onMenuFileOpen) );
    m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
          sigc::mem_fun(*this, &MainWindow::onMenuFileQuit));

    //Options menu:
    m_refActionGroup->add( Gtk::Action::create("OptionMenu", "Option") );
    m_refActionGroup->add( Gtk::Action::create("SpeedUp", "Increase ++", "Increase speed"),
                        sigc::mem_fun(*this, &MainWindow::onMenuSpeedUp) );
    m_refActionGroup->add( Gtk::Action::create("SpeedDown", "Decrease --", "Decrease speed"),
                        sigc::mem_fun(*this, &MainWindow::onMenuSpeedDown) );
    m_refActionGroup->add( Gtk::Action::create("SpeedNormal", "Normal 1x", "Normal speed"),
                        sigc::mem_fun(*this, &MainWindow::onMenuSpeedNormal) );
    m_refActionGroup->add( Gtk::ToggleAction::create("PlayBackRepeat", "_Repeat", "play in a loop", false),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackRepeat) );
    m_refActionGroup->add( Gtk::ToggleAction::create("PlayBackStrict", "_Strict"),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackStrict) );

    //Action menu:
    m_refActionGroup->add( Gtk::Action::create("PlayBackMenu", "Action") );
    m_refActionGroup->add( Gtk::Action::create("PlayBackPlay", Gtk::Stock::MEDIA_PLAY, "_Play", "Play data"),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackPlay) );
    m_refActionGroup->add( Gtk::Action::create("PlayBackPause", Gtk::Stock::MEDIA_PAUSE, "P_ause", "Pause data"),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackPause) );
    m_refActionGroup->add( Gtk::Action::create("PlayBackStop", Gtk::Stock::MEDIA_STOP, "_Stop", "Stop"),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackStop) );
    m_refActionGroup->add( Gtk::Action::create("PlayBackForward", Gtk::Stock::MEDIA_FORWARD, "_Forward", "Forward"),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackForward) );
    m_refActionGroup->add( Gtk::Action::create("PlayBackBackward", Gtk::Stock::MEDIA_REWIND, "_Rewind"),
                            sigc::mem_fun(*this, &MainWindow::onMenuPlayBackBackward) );
    //Help menu:
    m_refActionGroup->add( Gtk::Action::create("HelpMenu", "Help") );
    m_refActionGroup->add( Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
                            sigc::mem_fun(*this, &MainWindow::onMenuHelpAbout) );

    // initial sensitivity
    m_refActionGroup->get_action("PlayBackPause")->set_sensitive(false);
}

/**********************************************************/
bool MainWindow::onDeleteEvent(GdkEventAny* event)
{
    return !safeExit();
}
/**********************************************************/
void MainWindow::onMenuFileQuit()
{
    if(safeExit())
        hide();
}
/**********************************************************/
void MainWindow::onCommandQuit()
{
    if(cmdSafeExit())
        Gtk::Main::quit();
}
/**********************************************************/
bool MainWindow::cmdSafeExit(void)
{
    onMenuPlayBackStop();
    fprintf(stdout, "Module closing...\nCleaning up...\n");
    for (int x=0; x < subDirCnt; x++)
        utilities->partDetails[x].worker->release();
    fprintf(stdout, "Attempt to interrupt ports\n");
    for (int x=0; x < subDirCnt; x++)
        utilities->interruptPorts(utilities->partDetails[x]);
    fprintf(stdout, "Attempt to close ports\n");
    for (int x=0; x < subDirCnt; x++)
        utilities->closePorts(utilities->partDetails[x]);
    clearUtilities();
    fprintf(stdout, "Done!...\n");
    return true;
}
/**********************************************************/
bool MainWindow::safeExit(void)
{
    Gtk::MessageDialog dialog("Quit", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text("Do you want to quit?");
    if(dialog.run() == Gtk::RESPONSE_YES)
    {
        onMenuPlayBackStop();
        fprintf(stdout, "Module closing...\nCleaning up...\n");
        for (int x=0; x < subDirCnt; x++)
            utilities->partDetails[x].worker->release();
        fprintf(stdout, "Attempt to interrupt ports\n");
        for (int x=0; x < subDirCnt; x++)
            utilities->interruptPorts(utilities->partDetails[x]);
        fprintf(stdout, "Attempt to close ports\n");
        for (int x=0; x < subDirCnt; x++)
            utilities->closePorts(utilities->partDetails[x]);
        clearUtilities();
        fprintf(stdout, "Done!...\n");
        return true;
    }
    return false;
}

/**********************************************************/
void MainWindow::onMenuFileOpen()
{
    Gtk::FileChooserDialog dialog("Please choose a folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    if(dialog.run() == Gtk::RESPONSE_OK)
    {
        string fpath = dialog.get_filename();
        m_refTreeModel->clear();

        //for (int x=0; x < subDirCnt; x++)
            //utilities->interruptPorts(utilities->partDetails[x]);       
        for (int x=0; x < subDirCnt; x++)
            utilities->closePorts(utilities->partDetails[x]);  

        clearUtilities();
        createUtilities();

        doGuiSetup(fpath);
    }
}
/**********************************************************/

void MainWindow::onErrorMessage(const char *filename)
{
    Gtk::MessageDialog error("Setup Error", false /* use_markup */, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);

    string file = filename;
    string fullMessage = string( "There was a problem opening \n\n" + file + "\n\nplease check its validity");

    error.set_secondary_text(fullMessage.c_str());

    int result = error.run();
    //Handle the response:
    switch(result)
    {
        case(Gtk::RESPONSE_OK):
        {
            std::cout << "OK clicked." << std::endl;
            break;
        }
        default:
        {
            std::cout << "Unexpected button clicked." << std::endl;
            break;
        }
    }
        
}


/**********************************************************/
void MainWindow::onMenuHelpAbout()
{
    Gtk::AboutDialog dialog;
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 12)
    dialog.set_program_name("dataSetPlayer");
#endif

    dialog.set_version("1.00");
    dialog.set_copyright(
            "2012 (C) Robotics, Brain and Cognitive Sciences\n"
            "Istituto Italiano di Tecnologia");
    dialog.set_license("Permission is granted to copy, distribute, and/or modify \nthis program \
under the terms of the GNU General Public \nLicense,version 2 or any  \
later version published by the \nFree Software Foundation.\
\n\nA copy of the license can be found at\n \
http://www.robotcub.org/icub/license/gpl.txt");

    dialog.set_website("http://www.icub.org/");
    std::vector<Glib::ustring> authors;
    authors.push_back("Vadim Tikhanoff <vadim.tikhanoff@iit.it>");
    authors.push_back("Ali Paikan <ali.paikan@iit.it>");
    dialog.set_authors(authors);
    dialog.run();
}
/**********************************************************/
void MainWindow::onMenuPlayBackPlay()
{
    if (subDirCnt > 0)
    {
        m_playButton.set_stock_id(Gtk::Stock::MEDIA_PAUSE);
        m_refActionGroup->get_action("PlayBackPause")->set_sensitive(true);
        m_refActionGroup->get_action("PlayBackPlay")->set_sensitive(false);

        fprintf(stdout,"checking if port was changed by the user...\n");
        for (int i=0; i < subDirCnt; i++)
        {
            char test[256];
            getPartPort(utilities->partDetails[i].name.c_str(), test);
            if (strcmp( test , utilities->partDetails[i].portName.c_str()) == 0 )
                fprintf(stdout, "Port is the same continue\n");
            else
            {
                fprintf(stdout, "Modifying ports\n");
                utilities->partDetails[i].portName = test;//getPartPort( utilities->partDetails[i].name.c_str(), test);
                utilities->configurePorts(utilities->partDetails[i]);
            }
        }

        if ( utilities->masterThread->isRunning() )
        {
            fprintf(stdout,"asking the thread to resume\n");
            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].worker->resetTime();

            utilities->masterThread->resume();
        }
        else
        {
            fprintf(stdout,"asking the thread to start\n");
            fprintf(stdout,"initializing the workers...\n");
            for (int i=0; i < subDirCnt; i++)
                utilities->partDetails[i].worker->init();

            fprintf(stdout,"starting the master thread...\n");
            utilities->masterThread->start();
        }
        m_hPlayScale.set_sensitive(true);
    }
}
/**********************************************************/
void MainWindow::onMenuPlayBackPause()
{
    if (subDirCnt > 0)
    {
        m_playButton.set_stock_id(Gtk::Stock::MEDIA_PLAY);
        m_refActionGroup->get_action("PlayBackPause")->set_sensitive(false);
        m_refActionGroup->get_action("PlayBackPlay")->set_sensitive(true);

        fprintf(stdout, "asking the threads to pause...\n");
        utilities->masterThread->pause();
    }
}
/**********************************************************/
void MainWindow::resetButtonOnStop()
{
    m_playButton.set_stock_id(Gtk::Stock::MEDIA_PLAY);
}

/**********************************************************/
void MainWindow::onMenuPlayBackStop()
{
    if (subDirCnt > 0)
    {
        m_playButton.set_stock_id(Gtk::Stock::MEDIA_PLAY);
        fprintf(stdout, "asking the threads to stop...\n");
        if (utilities->masterThread->isSuspended())
            utilities->masterThread->resume();

        utilities->masterThread->stop();
        fprintf(stdout, "done stopping!\n");
        for (int i=0; i < subDirCnt; i++)
            utilities->partDetails[i].currFrame = 1;

        fprintf(stdout, "done stopping the thread...\n");
        m_hPlayScale.set_sensitive(false);
    }
}
/**********************************************************/
void MainWindow::onMenuPlayBackForward()
{
    if (subDirCnt > 0)
        utilities->masterThread->forward(5);
}
/**********************************************************/
void MainWindow::onMenuPlayBackBackward()
{
    if (subDirCnt > 0)
        utilities->masterThread->backward(5);
}
/**********************************************************/
void MainWindow::onMenuPlayBackStrict()
{
    Glib::RefPtr<Gtk::ToggleAction> act;
    act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
            m_refActionGroup->get_action("PlayBackStrict"));
    if(act && act->get_active())
    {
        fprintf(stdout,"strict mode is activated\n");
        utilities->sendStrict = true;
    }
    else
    {
        fprintf(stdout,"strict mode is deactivated\n");
        utilities->sendStrict = false;
    }
}
/**********************************************************/
void MainWindow::onMenuPlayBackRepeat()
{
    Glib::RefPtr<Gtk::ToggleAction> act;
    act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(
            m_refActionGroup->get_action("PlayBackRepeat"));
    if(act && act->get_active())
    {
        fprintf(stdout,"repeat mode is activated\n");
        utilities->repeat = true;
    }
    else
    {
        fprintf(stdout,"repeat mode is deactivated\n");
        utilities->repeat = false;
    }
}
/**********************************************************/
void MainWindow::onMenuSpeedUp()
{
    m_hScale.set_value(m_hScale.get_value() + 1.0);
}
/**********************************************************/
void MainWindow::onMenuSpeedDown()
{
    m_hScale.set_value(m_hScale.get_value() - 1.0);
}
/**********************************************************/
void MainWindow::onMenuSpeedNormal()
{
    m_hScale.set_value(1.0);
}
/**********************************************************/
void MainWindow::onPlayButtonTogle()
{
    if(m_playButton.get_stock_id() == Glib::ustring(Gtk::Stock::MEDIA_PLAY.id))
        onMenuPlayBackPlay();
    else
        onMenuPlayBackPause();
}
/**********************************************************/
void MainWindow::onSpeedValueChanged()
{
    double value = m_hScale.get_value();
    value = (value>=1.0) ? value : (value+1.0)/2.0;
    char szValue[16];
    sprintf(szValue, "%.1fx", value);
    m_labScaleValue.set_text(szValue);
    // the range is [0.25 ... 4.0]
    utilities->speed = value;
}
/**********************************************************/
void MainWindow::onPlayPositionSlided(Gtk::ScrollType scroll)
{
    fprintf(stdout, "slided ...\n");
}
/**********************************************************/
void MainWindow::onPlayPositionChanged()
{
    //double value = m_hPlayScale.get_value();
    //figure out which frame it is:
    //int selectedFrame = (int)( (utilities->partDetails[0].maxFrame * value) /100 );
    
    /*
     * this is currently commented as, a click on the bar creates events that are taken as a change in other functions
     * Still to find a way to fix it...
     */

    /*for (int i=0; i < subDirCnt; i++)
    {
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * value) /100 );
        if ( (utilities->partDetails[i].currFrame < utilities->partDetails[i].maxFrame) && (utilities->partDetails[i].currFrame > 1 ) )
        {
            utilities->partDetails[i].currFrame = selectedFrame;
            if (i == 0)
            {
                utilities->masterThread->virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
            }
        }
        else
            fprintf(stdout, "cannot go any forward, out of range..\n");
    }*/
}
/**********************************************************/
bool MainWindow::getRowByPart(const char* szName, Gtk::TreeModel::Row* row )
{
    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refTreeModel->children();
    for(type_children::iterator iter = children.begin(); iter!=children.end(); ++iter)
    {
        if((*iter)[m_partColumns.m_col_part] == Glib::ustring(szName))
        {
            *row = (*iter);
            return true;
        }
    }
    return false;
}
/**********************************************************/
void MainWindow::onTreeRowChanged(const Gtk::TreeModel::Path& path,
                      const Gtk::TreeModel::iterator& iter)
{
    Glib::ustring part = (*iter)[m_partColumns.m_col_part];
    bool activated = (*iter)[m_partColumns.m_col_enabled];

    //part activation
    if(part.size())
    {
        cout<<part;
        if(activated)
            cout<<" is Active!"<<endl;
        else
            cout<<" is Disactive!"<<endl;
    }
}
/**********************************************************/
bool MainWindow::setFrameRate(const char* szName, int frameRate)
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    bool ret = getRowByPart(szName, &row);
    if(ret)
    {
        char str[16];
        sprintf(str,"%d ms",frameRate);
        row[m_partColumns.m_col_frameRate] = str;
    }
        GDK_THREADS_LEAVE();

    return ret;
}
/**********************************************************/
bool MainWindow::setInitialPartProgress(const char* szName, int percentage)
{
    Gtk::TreeModel::Row row;
    bool ret = getRowByPart(szName, &row);
    if(ret)
        row[m_partColumns.m_col_percentage] = percentage;

    return ret;
}
/**********************************************************/
bool MainWindow::setPartProgress(const char* szName, int percentage)
{
    GDK_THREADS_ENTER();
    Gtk::TreeModel::Row row;
    bool ret = getRowByPart(szName, &row);
    if(ret)
        row[m_partColumns.m_col_percentage] = percentage;
    GDK_THREADS_LEAVE();

    return ret;
}
/**********************************************************/
void MainWindow::setPlayProgress(int percentage)
{
    GDK_THREADS_ENTER();
    m_hPlayScale.set_value(percentage);
    GDK_THREADS_LEAVE();
}
/**********************************************************/
bool MainWindow::getPartActivation(const char* szName)
{
    Gtk::TreeModel::Row row;
    bool ret = getRowByPart(szName, &row);
    if(ret)
        ret = row[m_partColumns.m_col_enabled];
    return ret;
}
/**********************************************************/
bool MainWindow::getPartPort(const char* szName, char* dest)
{
    Gtk::TreeModel::Row row;
    bool ret = getRowByPart(szName, &row);
    if(ret)
    {
        Glib::ustring str = row[m_partColumns.m_col_port];
        sprintf(dest,"%s",str.c_str());
        return true;
    }
    fprintf(stdout,"returning null\n");
    return false;
}
/**********************************************************/



