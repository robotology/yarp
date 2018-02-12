/*
 * Copyright (C) 2007 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 * Originally based on "wx-sdl tutorial v1.1", copyright follows
 *
 */

/*******************************************************************************
 * Based on:
 * wx-sdl tutorial v1.1
 * Copyright (C) 2005,2007 John David Ratliff
 *
 * wx-sdl is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wx-sdl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wx-sdl; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************/

#include <iostream>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>
#include <wx/image.h>

#include "SDL.h"

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

/*******************************************************************************
 * Global Declarations
*******************************************************************************/

enum {
    IDF_FRAME = wxID_HIGHEST + 1,
    IDP_PANEL
};

/*******************************************************************************
 * SDLPanel Class
*******************************************************************************/

static bool wxsdl_stopped = false;
static bool wxsdl_drawing = false;
static int wxsdl_post = 0;
static bool wxsdl_framed = true;
static int __width = 200;
static int __height = 200;
static int __x = wxDefaultPosition.x;
static int __y = wxDefaultPosition.y;
static ConstString __title = "yarpview!";
static ConstString __clicker = "";
static bool wxsdl_running = false;

static Semaphore mutex(1), finished(0);

class SDLPanel : public wxPanel {
    DECLARE_CLASS(SDLPanel)
    DECLARE_EVENT_TABLE()
    
private:
    SDL_Surface *screen;
    int wscreen, hscreen;
    int wwin, hwin;
    //Semaphore mutex;
    bool stopped;
    BufferedPort<Bottle> clicks;
    bool clicking;
    int wshow, hshow;

    /**
     * Called to paint the panel.
     */
    void onPaint(wxPaintEvent &);
    
    /**
     * Called to erase the background.
     */
    void onEraseBackground(wxEraseEvent &);
    
    /**
     * Called to update the panel in idle time.
     */
    void onIdle(wxIdleEvent &);
    

    void onClick(wxMouseEvent & event);

    /**
     * Creates the SDL_Surface used by this SDLPanel.
     */
    void createScreen(int w, int h);

public:
    /**
     * Creates a new SDLPanel.
     *
     * @param parent The wxWindow parent.
     */
    SDLPanel(wxWindow *parent);
    
    /**
     * Destructor for the SDLPanel.
     */
    ~SDLPanel();

    void putImage(ImageOf<PixelRgb>& image);

    void stop() {
        mutex.wait();
        stopped = true;
        wxsdl_stopped = true;
        if (screen) {
            SDL_FreeSurface(screen);
            screen = NULL;
        }
        mutex.post();
    }
};

inline void SDLPanel::onEraseBackground(wxEraseEvent &) { /* do nothing */ }

IMPLEMENT_CLASS(SDLPanel, wxPanel)

BEGIN_EVENT_TABLE(SDLPanel, wxPanel)
    EVT_PAINT(SDLPanel::onPaint)
    EVT_ERASE_BACKGROUND(SDLPanel::onEraseBackground)
    EVT_IDLE(SDLPanel::onIdle)
    EVT_LEFT_DOWN(SDLPanel::onClick)
END_EVENT_TABLE()

SDLPanel::SDLPanel(wxWindow *parent) : wxPanel(parent, IDP_PANEL), screen(0) {

    wscreen = hscreen = 0;
    stopped = false;
    clicking = false;

    if (__clicker != "") {
        clicks.open(__clicker);
        clicking = true;
    }

    /*
    // ensure the size of the wxPanel
    wxSize minsize(10, 10);
    wxSize maxsize(640, 480);
    
    SetMinSize(minsize);
    SetMaxSize(maxsize);
    */
}

SDLPanel::~SDLPanel() {
    if (clicking) {
        clicks.close();
    }
    wxsdl_stopped = true;
    int ct = 10;
    while (wxsdl_drawing&&ct>0) {
        printf("sdl panel dying\n");
        SystemClock::delaySystem(0.1);
        ct--;
    }
    if (ct==0) {
        printf("Huh, that was unusually long...\n");
    }

    mutex.wait();
    printf("sdl panel dead\n");

    if (screen) {
        SDL_FreeSurface(screen);
        screen = NULL;
    }
    mutex.post();
}

void SDLPanel::onPaint(wxPaintEvent& ev) {
    bool done = false;

    //printf("taking permit\n");

    mutex.wait();

    // can't draw if the screen doesn't exist yet
    if (screen) {
    
        if (!done) {
            // create a bitmap from our pixel data
            wxImage img(screen->w, screen->h, 
                        static_cast<unsigned char *>(screen->pixels), true);

            int width, height;
            GetClientSize(&width, &height);
            wwin = width;
            hwin = height;

            img.Rescale(width,height);

            wxBitmap bmp(img);
                         
            // paint the screen
            wxBufferedPaintDC dc(this, bmp);
        } else {
            wxPaintDC dc(this);
            dc.Clear();
        }
    } else {
        wxPaintDC dc(this);
        dc.Clear();
    }

    mutex.post();

    //printf("returning permit\n");

}

void SDLPanel::onIdle(wxIdleEvent &) {
    return;
}


void SDLPanel::onClick(wxMouseEvent & event) {
    if (clicking) {
        mutex.wait();
        int cx = event.m_x;
        int cy = event.m_y;
        if (wwin!=wshow && wwin!=0) {
            cx = (int)(cx*(((double)wshow)/wwin));
        }
        if (hwin!=hshow && hwin!=0) {
            cy = (int)(cy*(((double)hshow)/hwin));
        }
        mutex.post();
        /*
        printf("got clicked %d %d / %d %d / %d %d\n", cx, cy,
               wwin, hwin,
               wshow, hshow);
        */
        Bottle& bot = clicks.prepare();
        bot.clear();
        bot.addInt(cx);
        bot.addInt(cy);
        clicks.write();
    }
    return;
}


void SDLPanel::putImage(ImageOf<PixelRgb>& image) {

    if (image.width()==0) return;

    //printf("%d %d // %d %d\n",
    //image.width(),image.height(),
    //wscreen, hscreen);
    //fflush(stdout);

    //printf("low level put image\n");

    if (!wxsdl_running) {
        //printf("WAITING for wxsdl\n");
        SystemClock::delaySystem(0.1);
    }

    mutex.wait();
    wshow = image.width();
    hshow = image.height();

    if (stopped||wxsdl_stopped) {
        mutex.post();
        return;
    }

    //printf("got permit\n");

    // create the SDL_Surface
    createScreen(image.width(),image.height());
    

    bool done = false;

    //printf("%d %d // %d %d\n",
    //image.width(),image.height(),
    //wscreen, hscreen);
    //fflush(stdout);

    if (!done) {
        for (int y = 0; y < hscreen; y++) {
            for (int x = 0; x < wscreen; x++) {
                //wxUint32 color = (y * y) + (x * x) + tick;
                wxUint8 *pixels = static_cast<wxUint8 *>(screen->pixels) + 
                    (y * screen->pitch) +
                    (x * screen->format->BytesPerPixel);
                
                if (x<image.width()&&y<image.height()) {
                    PixelRgb& pix = image(x,y);
                    wxUint8 rr = (wxUint8) pix.r;
                    wxUint8 gg = (wxUint8) pix.g;
                    wxUint8 bb = (wxUint8) pix.b;
                    
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    pixels[0] = bb; //color & 0xFF;
                    pixels[1] = gg; //(color >> 8) & 0xFF;
                    pixels[2] = rr; //(color >> 16) & 0xFF;
#else
                    pixels[0] = rr; //(color >> 16) & 0xFF;
                    pixels[1] = gg; //(color >> 8) & 0xFF;
                    pixels[2] = bb; //color & 0xFF;
#endif
                }
            }
        }
        
        mutex.post();

#if defined(_WIN32)
        Refresh();
#else
        wxMutexGuiEnter();
        wxPaintEvent paintEv;
        wxPostEvent(this,paintEv);
        wxMutexGuiLeave();
#endif
    } else {

        mutex.post();
    }
}


void SDLPanel::createScreen(int w, int h) {
    if (screen && (w!=wscreen || h!=hscreen)) {
        SDL_FreeSurface(screen);
        screen = NULL;
    }
    if (!screen) {
        //int width, height;
        //GetSize(&width, &height);

        wscreen = w;
        hscreen = h;

        screen = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 
                                      24, 0, 0, 0, 0);     
    }
}

/*******************************************************************************
// SDLFrame Class
*******************************************************************************/

class SDLFrame : public wxFrame {
    DECLARE_CLASS(SDLFrame)
    DECLARE_EVENT_TABLE()
    
private:
    SDLPanel *panel;
    
    /**
     * Called when exit from the file menu is selected.
     */
    void onFileExit(wxCommandEvent &);
    
    /**
     * Called when about from the help menu is selected.
     */
    void onHelpAbout(wxCommandEvent &);
    
public:
    /**
     * Creates a new SDLFrame.
     */
    SDLFrame();

    virtual ~SDLFrame() {
        mutex.wait();
        printf("frame gone\n");
        wxsdl_framed = false;
        mutex.post();
    }

    /**
     * Gets the SDLPanel within this SDLFrame.
     *
     * @return The SDLPanel.
     */
    SDLPanel &getPanel();


    bool putImage(ImageOf<PixelRgb>& image) {
        if (panel!=NULL) {
            //printf("put image on panel\n");
            panel->putImage(image);
            return true;
        }
        printf("No panel to draw on\n");
        return true;
    }
};


static void wxsdlStop() {
    mutex.wait();
    bool working = wxsdl_drawing;
    wxsdl_stopped = true;
    if (working) {
        wxsdl_post = 1;
    }
    mutex.post();
    if (working) {
        finished.wait();
    }
    printf("external writer stopped\n");
}

inline void SDLFrame::onFileExit(wxCommandEvent &) { 
    printf("On file exit\n");
    wxsdlStop();
    Close(); 
}

inline SDLPanel &SDLFrame::getPanel() { return *panel; }

IMPLEMENT_CLASS(SDLFrame, wxFrame)

BEGIN_EVENT_TABLE(SDLFrame, wxFrame)
    EVT_MENU(wxID_EXIT, SDLFrame::onFileExit)
    EVT_MENU(wxID_ABOUT, SDLFrame::onHelpAbout)
END_EVENT_TABLE()

SDLFrame::SDLFrame() {
    // Create the SDLFrame
    //printf("position %d %d\n", __x, __y);
    Create(0, IDF_FRAME, wxString(__title.c_str(),wxConvUTF8), 
           wxPoint(__x,__y),
           wxDefaultSize, wxDEFAULT_FRAME_STYLE);

           // wxCAPTION | wxSYSTEM_MENU | 
           //wxMINIMIZE_BOX | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BOX);

    // create the main menubar
    wxMenuBar *mb = new wxMenuBar;
    
    // create the file menu
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
    
    // add the file menu to the menu bar
    mb->Append(fileMenu, wxT("&File"));
    
    // create the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, wxT("About"));
    
    // add the help menu to the menu bar
    mb->Append(helpMenu, wxT("&Help"));
    
    // add the menu bar to the SDLFrame
    SetMenuBar(mb);
    
    // create the SDLPanel
    panel = new SDLPanel(this);
}

void SDLFrame::onHelpAbout(wxCommandEvent &) {
    wxMessageBox(wxT("WxsdlWriter is based on the wx-sdl tutorial\nCopyright (C) 2005,2007 John Ratliff\nAdaptations to YARP are (C) 2007 Paul Fitzpatrick\nReleased under GPLv2"),
                 wxT("about yarpview"), wxOK | wxICON_INFORMATION);
}

/*******************************************************************************
// SDLApp Class
*******************************************************************************/

class SDLApp : public wxApp {
    DECLARE_CLASS(SDLApp)
    
private:
    SDLFrame *frame;
    
public:

    SDLFrame *getFrame() {
        return frame;
    }

    /**
     * Called to initialize this SDLApp.
     *
     * @return true if initialization succeeded; false otherwise.
     */
    bool OnInit();
    
    /**
     * Called to run this SDLApp.
     *
     * @return The status code (0 if good, non-0 if bad).
     */
    int OnRun();
    
    /**
     * Called when this SDLApp is ready to exit.
     *
     * @return The exit code.
     */
    int OnExit();


    bool putImage(ImageOf<PixelRgb>& image) {
        if (frame!=NULL) {
            //printf("put image on frame\n");
            return frame->putImage(image);
        }
        printf("No frame to draw on\n");
        return true;
    }
};

bool SDLApp::OnInit() {
    // create the SDLFrame
    frame = new SDLFrame;
    frame->SetClientSize(__width, __height);
    //frame->Centre();
    frame->Show();
    
    // Our SDLFrame is the Top Window
    SetTopWindow(frame);

    // initialization should always succeed
    return true;
}

int SDLApp::OnRun() {
  printf("starting...\n");
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "unable to initialize SDL: " << SDL_GetError() << '\n';
        
        return -1;
    }
    printf("initialized\n");

    printf("running\n");
    fflush(stdout);
    wxsdl_running = true;
    
    // generate an initial idle event to start things
    //wxIdleEvent event;
    //event.SetEventObject(&frame->getPanel());
    //frame->getPanel().AddPendingEvent(event);

    // start the main loop
    return wxApp::OnRun();
}

int SDLApp::OnExit() {
    wxsdlStop();
    printf("standard exit\n");
    return wxApp::OnExit();
}

IMPLEMENT_CLASS(SDLApp, wxApp)

IMPLEMENT_APP_NO_MAIN(SDLApp)
    //IMPLEMENT_APP(SDLApp)



#include "WxsdlWriter.h"

using namespace yarp::dev;


void WxsdlWriter::run() {
    int argc = 1;
    char *argv[] = { "yarp", NULL };
    active = true;
    wxEntry(argc, argv);
    active = false;

    printf("sdl shutdown\n");
    SDL_Quit();

    printf("WxsdlWriter finished\n");
}

bool WxsdlWriter::open(yarp::os::Searchable & config) {
    int width = config.check("w",
                             Value(128),
                             "width of viewer").asInt();
    if (!config.check("w")) {
        width = config.check("width",
                             Value(128),
                             "width of viewer").asInt();
    }
    int height = config.check("h",
                              Value(128),
                              "height of viewer").asInt();
    if (!config.check("h")) {
        height = config.check("height",
                              Value(128),
                              "height of viewer").asInt();
    }
    __title = config.check("title",
                           Value("yarpview"),
                           "title of viewer").asString();
    if (!config.check("title")) {
        __title = config.check("name",
                               Value("yarpview"),
                               "titile of viewer").asString();
    }

    if (config.check("x")) {
        __x = config.check("x",
                           Value(0),
                           "x coordinate of viewer").asInt();
    }

    if (config.check("y")) {
        __y = config.check("y",
                           Value(0),
                           "y coordinate of viewer").asInt();
    }

    if (config.check("out")) {
        __clicker = config.check("out",
                                 Value(""),
                                 "port name for clicks").asString();
    }

    __width = width;
    __height = height;
    start();
    return true;
}
    
bool WxsdlWriter::close() {
    printf("WxsdlWriter::close\n");
    mutex.wait();
    if (active) {
        active = false; 
        mutex.post();
        wxsdlStop();
        wxMutexGuiEnter();
        if (wxsdl_framed) {
            wxCloseEvent closeEv;
            wxPostEvent(wxGetApp().getFrame(), closeEv);
            // wxGetApp().CleanUp();
        }
        wxMutexGuiLeave();
        stop();
    } else {
        mutex.post();
    }

    return true;
}
  
bool WxsdlWriter::putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) {

    // just in case
    //printf("WAITING for wxsdl\n");
    if (!wxsdl_running) {
        //printf("WAITING for wxsdl\n");
        SystemClock::delaySystem(0.1);
    }
    //printf("FINISHED WAITING for wxsdl\n");


    mutex.wait();
    if (!active) {
        mutex.post();
        return false;
    }
    if (wxsdl_stopped) {
        mutex.post();
        return false;
    }

    wxsdl_drawing = true;
    mutex.post();

    //printf("Putting image\n");
    bool result = wxGetApp().putImage(image);

    mutex.wait();
    wxsdl_drawing = false;
    while (wxsdl_post>0) {
        finished.post();
        wxsdl_post--;
    }
    mutex.post();

    return result;
}


bool WxsdlWriter::updateService() {
    // could be a lot smarter here...
    SystemClock::delaySystem(1);
    return !wxsdl_stopped;
}
    
bool WxsdlWriter::stopService() {
    return close();
}


#ifdef WXSDL_MAIN

int main() {
    Network yarp;
 
    bool done = false;

    PolyDriver source;
    Property pSource;
    pSource.put("device","test_grabber");
    source.open(pSource);
    IFrameGrabberImage *iSource;
    source.view(iSource);
    if (iSource==NULL) {
        printf("Cannot find image source\n");
        return 1;
    }

    WxsdlWriter writer;
    Property p;
    writer.open(p);

    ImageOf<PixelRgb> img;
    while (!done) {
        
        iSource->getImage(img);
        bool ok = writer.putImage(img);
        if (!ok) {
            done = true;
        } 
    }
    writer.close();

    printf("Wxsdl test program finished\n");

    return 0;
}

#endif

