/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

#include <list>
#include <gtkmm.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>

#include <yarp/manager/manager.h>

#include "message_list.h"
#include "application_list.h"
#include "module_preview_window.h"


class MainWindow : public Gtk::Window
{

public:
    MainWindow( yarp::os::Property &config);
    virtual ~MainWindow();

    void onTabCloseRequest(Widget* wdg);
    //Glib::RefPtr<MessagesList> m_refMessageList;

    void reportErrors(void);

    yarp::manager::Manager* getManager(void) {return &lazyManager;}

public:
    MessagesList* m_refMessageList;
    ApplicationList* m_refApplicationList;
    ModulePreviewWindow* m_refModPreview;

    std::list<Gtk::TargetEntry> dragTargets;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    yarp::os::Property m_config;

protected:

    //Signal handlers:
    bool onExposeEvent(GdkEventExpose* event);
    void onMenuFileQuit();
    void onMenuFileNewApp();
    void onMenuFileNewMod();
    void onMenuFileNewRes();
    void onMenuFileOpen();
    void onMenuFileClose();
    void onMenuFileOpenGymanager();
public:
    void onMenuFileSave();
protected:
    void onMenuFileSaveAs();
    void onMenuFileImport();
    void onMenuHelpAbout();
    void onMenuHelpOnlineHelp();
    void onMenuEditUndo();
    void onMenuEditRedo();
    void onMenuEditCut();
    void onMenuEditCopy();
    void onMenuEditDelete();
    void onMenuEditPaste();
    void onMenuEditSellAll();
    void onMenuEditExportGraph();
    void onMenuViewZoomIn();
    void onMenuViewZoomOut();
    void onMenuViewZoomReset();
    void onMenuViewGrid();
    void onMenuViewSnapToGrid();
    void onMenuViewLabel();
    void onMenuWindowItem();
    void onMenuWindowMessage();
    void onMenuWindowProperty();
    void onMenuInsertSrcPort();
    void onMenuInsertDestPort();
    void onMenuInsertPortArbitrator();
    void onMenuRotateRight();
    void onMenuRotateLeft();

    void onAppListRowActivated(const Gtk::TreeModel::Path& path,
                Gtk::TreeViewColumn* column);
    void onAppListButtonPressed(GdkEventButton* event);
    void onAppListCursorChanged();

    void onNotebookSwitchPage(GtkNotebookPage* page, guint page_num);
    bool onDeleteEvent(GdkEventAny* event);

    void onPAppMenuLoad();
    void onPAppMenuRemove();
    void onPAppMenuReopen();
    void onEditFile();

    void on_size_allocate(Gtk::Allocation& allocation);

private:
    yarp::manager::Manager lazyManager;

    //Child widgets:
    int dummy_w;
    int dummy_h;
    Gtk::VBox m_VBox;
    Gtk::VPaned m_VPaned;
    Gtk::HPaned m_HPaned;
    Gtk::Statusbar m_Statusbar;
    Gtk::Notebook m_bottomTab;
    Gtk::Notebook m_mainTab;
    Gtk::Frame frame1;
    Gtk::TextView m_commandView;
    Glib::RefPtr<Gtk::TextBuffer> m_refCommandBuffer;
    Glib::RefPtr<Gtk::IconFactory> m_factory;

    void manageApplication(const char* szName);
    void manageResource(const char* szName);
    void manageModule(const char* szName);
    void manageTemplate(const char* szName);

    bool safeExit(void);
    void closeTab(int page_num);
    void createWidgets(void);
    void setupStocks(void);
    void setupActions(void);
    void setupSignals(void);
    bool loadRecursiveApplications(const char* szPath);
    bool loadRecursiveTemplates(const char* szPath);
    void syncApplicationList(void);
};



#endif //_MAIN_WINDOW_

