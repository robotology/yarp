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

class MainWindow : public Gtk::Window, public yarp::os::ResourceFinder, public yarp::os::Module
{
public:
    MainWindow(yarp::os::ResourceFinder    &rf);
    virtual ~MainWindow();
    void addPart(const char* szName, const char* type, int frames, const char* portName, const char* szFileName=NULL);
    bool setPartProgress(const char* szName, int percentage);
    bool setInitialPartProgress(const char* szName, int percentage);
    bool setFrameRate(const char* szName, int frameRate);
    void setPlayProgress(int percentage);
    bool getPartActivation(const char* szName);
    void onPlayButtonTogle();
    void onMenuPlayBackStop();
    void onMenuPlayBackPlay();
    void resetButtonOnStop();
    void onMenuPlayBackPause();
    bool getPartPort(const char* szName, char* dest);
    bool execReq(const yarp::os::Bottle &command, yarp::os::Bottle &reply);
    bool respond(const yarp::os::Bottle &command, yarp::os::Bottle &reply)
    {
        if(execReq(command,reply))
            return true;
        else
            return yarp::os::Module::respond(command,reply);
    }
protected:
    bool updateFrameNumber(const char* part, int number);
    int getFrame(const char* part);
    void stepFromCommand(yarp::os::Bottle &reply);
    void listOfCommands(yarp::os::Bottle &reply);

    //Signal handlers:
    void onCommandQuit();
    void onMenuFileQuit();
    void onMenuFileOpen();
    void onMenuHelpAbout();
    void onMenuPlayBackForward();
    void onMenuPlayBackBackward();
    void onMenuPlayBackStrict();
    void onMenuPlayBackRepeat();
    void onMenuSpeedUp();
    void onMenuSpeedDown();
    void onMenuSpeedNormal();
    void onSpeedValueChanged();
    void onPlayPositionChanged();
    void onPlayPositionSlided(Gtk::ScrollType scroll);
    void onTreeRowChanged(const Gtk::TreeModel::Path& path,
                          const Gtk::TreeModel::iterator& iter);
    bool onDeleteEvent(GdkEventAny* event);

    Utilities                   *utilities;
    friend class                Utilities;
    std::map<const char*,int>   partMap;
    int                         itr;

    void createUtilities();
    void clearUtilities();

private:
    std::string                 moduleName; //string containing module name
    bool                        add_prefix; //indicates if ports have to be opened with /<moduleName> as prefix
    yarp::os::Port              rpcHuman;

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
    std::vector<std::string>    partsLogPath;
    int                         subDirCnt;
    std::vector<std::string>    dataType;

    int initialize(void);
    int doGuiSetup(std::string newPath);
    void createWidgets(void);
    void setupActions(void);
    void setupSignals(void);
    bool safeExit(void);
    bool cmdSafeExit(void);
    bool getRowByPart(const char* szName, Gtk::TreeModel::Row* row );
};

#endif //_MAIN_WINDOW_

