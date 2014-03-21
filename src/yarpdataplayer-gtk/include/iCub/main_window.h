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

#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_
#include <string>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <gtkmm.h>
#include "iCub/utils.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Module.h>
#include <yarp/os/RpcServer.h>
#include "dataSetPlayer_IDL.h"

/**********************************************************/
class PartModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:

    PartModelColumns() {
        add(m_col_enabled);
        add(m_col_part);
        add(m_col_type);
        add(m_col_frames);
        add(m_col_frameRate);
        add(m_col_percentage);
        add(m_col_filename);
        add(m_col_port);
    }
    Gtk::TreeModelColumn<bool> m_col_enabled;
    Gtk::TreeModelColumn<Glib::ustring> m_col_part;
    Gtk::TreeModelColumn<int> m_col_frames;
    Gtk::TreeModelColumn<Glib::ustring> m_col_frameRate;
    Gtk::TreeModelColumn<int> m_col_percentage;
    Gtk::TreeModelColumn<Glib::ustring> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_filename;
    Gtk::TreeModelColumn<Glib::ustring> m_col_port;
};

/**********************************************************/
class MainWindow : public Gtk::Window, public yarp::os::ResourceFinder, public yarp::os::Module, public dataSetPlayer_IDL
{
public:
    MainWindow(yarp::os::ResourceFinder    &rf);
    virtual ~MainWindow();
    /**
     * function that adds a data part to the main window
     */
    void addPart(const char* szName, const char* type, int frames, const char* portName, const char* szFileName=NULL);
    /**
     * function that sets the dataset part progress bar
     */
    bool setPartProgress(const char* szName, int percentage);
    /**
     * function that sets the initial dataset part progress bar
     */
    bool setInitialPartProgress(const char* szName, int percentage);
    /**
     * function that sets the frame rate
     */
    bool setFrameRate(const char* szName, int frameRate);
    /**
     * function that sets the play progress bar
     */
    void setPlayProgress(int percentage);
    /**
     * function that gets which parts are activated
     */
    bool getPartActivation(const char* szName);
    /**
     * function that handles the play button toggle
     */
    void onPlayButtonTogle();
    /**
     * function that handles the stop menu toggle
     */
    void onMenuPlayBackStop();
    /**
     * function that handles the play menu toggle
     */
    void onMenuPlayBackPlay();
    /**
     * function that handles the reset button toggle
     */
    void resetButtonOnStop();
    /**
     * function that handles the play/pause menu toggle
     */
    void onMenuPlayBackPause();
    /**
     * function that provides the error message
     */
    void onErrorMessage(const char *filename);
    /**
     * function that handles individual dataset part ports
     */
    bool getPartPort(const char* szName, char* dest);

    /**
     * function that that attaches the rpcServer port for IDL
     */
    bool attach(yarp::os::RpcServer &source);
    /**
     * function that that handles an IDL message - step
     */
    bool step();
    /**
     * function that handles an IDL message - setFrame
     */
    bool setFrame(const std::string &name, const int frameNum);
    /**
     * function that handles an IDL message - getFrame
     */
    int  getFrame(const std::string &name);
    /**
     * function that handles an IDL message - load
     */
    bool load(const std::string &path);
    /**
     * function that handles an IDL message - play
     */
    bool play();
    /**
     * function that handles an IDL message - pause
     */
    bool pause();
    /**
     * function that handles an IDL message - stop
     */
    bool stop();
    /**
     * function that handles an IDL message - quit
     */
    bool quit();

protected:
    /**
     * function that updates the frame number
     */
    bool updateFrameNumber(const char* part, int number);
    /**
     * function that gets the frame command
     */
    int  getFrameCmd(const char* part);
    /**
     * function steps datasets when requeted from terminal
     */
    void stepFromCommand(yarp::os::Bottle &reply);
    /**
     * signal handle function - quit
     */
    void onCommandQuit();
    /**
     * signal handle function - menu quit
     */
    void onMenuFileQuit();
    /**
     * signal handle function - menu open
     */
    void onMenuFileOpen();
    /**
     * signal handle function - menu help
     */
    void onMenuHelpAbout();
    /**
     * signal handle function - menu forward
     */
    void onMenuPlayBackForward();
    /**
     * signal handle function - menu backward
     */
    void onMenuPlayBackBackward();
    /**
     * signal handle function - menu send strict
     */
    void onMenuPlayBackStrict();
    /**
     * signal handle function - menu repeat
     */
    void onMenuPlayBackRepeat();
    /**
     * signal handle function - menu speed up
     */
    void onMenuSpeedUp();
    /**
     * signal handle function - menu speed down
     */
    void onMenuSpeedDown();
    /**
     * signal handle function - menu default speed
     */
    void onMenuSpeedNormal();
    /**
     * signal handle function - speed changed
     */
    void onSpeedValueChanged();
    /**
     * signal handle function - position changed
     */
    void onPlayPositionChanged();
    /**
     * signal handle function - position slided
     */
    void onPlayPositionSlided(Gtk::ScrollType scroll);
    /**
     * Tree row modified function
     */
    void onTreeRowChanged(const Gtk::TreeModel::Path& path,
                          const Gtk::TreeModel::iterator& iter);
    /**
     * function that handles delete events
     */
    bool onDeleteEvent(GdkEventAny* event);

    Utilities                   *utilities;
    friend class                Utilities;
    std::map<const char*,int>   partMap;
    int                         itr;
    int                         column;
    bool                        withExtraTimeCol;

    /**
     * function that creates utilities
     */
    void createUtilities();
    /**
     * function that deletes utilities
     */
    void clearUtilities();

private:
    std::string                 moduleName; //string containing module name
    bool                        add_prefix; //indicates if ports have to be opened with /<moduleName> as prefix
    yarp::os::RpcServer         rpcPort;

    //Child widgets:
    Gtk::VBox m_VBox;
    Gtk::HBox m_HBox;
    Gtk::Statusbar m_Statusbar;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

    Gtk::TreeView m_TreeView;
    Gtk::ScrolledWindow m_ScrollView;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    PartModelColumns m_partColumns;
    Gtk::TreeModel::Row m_partRow;

    Gtk::ToolButton m_playButton;
    Gtk::ToolButton m_stopButton;
    Gtk::ToolButton m_forwardButton;
    Gtk::ToolButton m_rewindButton;

    Gtk::Label m_labScale;
    Gtk::Label m_labScaleValue;
    Gtk::HScale m_hScale;
    Gtk::HScale m_hPlayScale;

    std::vector<std::string>    partsName;
    std::vector<std::string>    partsFullPath;
    std::vector<std::string>    partsInfoPath;
    std::vector<std::string>    partsLogPath;
    int                         subDirCnt;
    std::vector<std::string>    dataType;

    /**
     * function that initializes the main window
     */
    int initialize(void);
    /**
     * function that setups the main window
     */
    int doGuiSetup(std::string newPath);
    /**
     * function that creates the required widgets
     */
    void createWidgets(void);
    /**
     * function that setups the required actions
     */
    void setupActions(void);
    /**
     * function that setups the required signals
     */
    void setupSignals(void);
    /**
     * function that closes the module from the gui
     */
    bool safeExit(void);
    /**
     * function that closes the module from the terminal
     */
    bool cmdSafeExit(void);
    /**
     * function that accesses the dataset row by part
     */
    bool getRowByPart(const char* szName, Gtk::TreeModel::Row* row );
};

#endif //_MAIN_WINDOW_

