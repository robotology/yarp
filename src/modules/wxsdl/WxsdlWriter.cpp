// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 * Originally based on "wx-sdl tutorial v1.1", copyright follows
 *
 */

/*******************************************************************************
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
using namespace yarp::os;
using namespace yarp::sig;

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

static Semaphore mutex(1);

class SDLPanel : public wxPanel {
    DECLARE_CLASS(SDLPanel)
    DECLARE_EVENT_TABLE()
    
private:
    SDL_Surface *screen;
    int wscreen, hscreen;
    //Semaphore mutex;
    bool stopped;

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
END_EVENT_TABLE()

SDLPanel::SDLPanel(wxWindow *parent) : wxPanel(parent, IDP_PANEL), screen(0) {

    wscreen = hscreen = 0;
    stopped = false;

    /*
    // ensure the size of the wxPanel
    wxSize minsize(10, 10);
    wxSize maxsize(640, 480);
    
    SetMinSize(minsize);
    SetMaxSize(maxsize);
    */
}

SDLPanel::~SDLPanel() {
    wxsdl_stopped = true;
    while (wxsdl_drawing) {
        printf("sdl panel dying\n");
        Time::delay(0.1);
    }
    printf("sdl panel dead\n");

    if (screen) {
        SDL_FreeSurface(screen);
        screen = NULL;
    }
}

void SDLPanel::onPaint(wxPaintEvent &) {
    bool done = false;

    //printf("taking permit\n");

    mutex.wait();

    // can't draw if the screen doesn't exist yet
    if (screen) {
    
        // lock the surface if necessary
        if (SDL_MUSTLOCK(screen)) {
            if (SDL_LockSurface(screen) < 0) {
                done = true;
            }
        }
    
        if (!done) {
            // create a bitmap from our pixel data
            wxImage img(screen->w, screen->h, 
                        static_cast<unsigned char *>(screen->pixels), true);

            int width, height;
            GetClientSize(&width, &height);

            img.Rescale(width,height);

            wxBitmap bmp(img);
                         
            
            // unlock the screen
            if (SDL_MUSTLOCK(screen)) {
                SDL_UnlockSurface(screen);
            }
            
            // paint the screen
            wxBufferedPaintDC dc(this, bmp);
        }
    }

    mutex.post();

    //printf("returning permit\n");

}

void SDLPanel::onIdle(wxIdleEvent &) {
    return;
}


void SDLPanel::putImage(ImageOf<PixelRgb>& image) {
    //printf("low level put image\n");


    mutex.wait();

    if (stopped||wxsdl_stopped) {
        mutex.post();
        return;
    }

    //printf("got permit\n");

    // create the SDL_Surface
    createScreen(image.width(),image.height());
    

    bool done = false;

    // Lock surface if needed
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) < 0) {
            done = true;
        }
    }
    
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
        
        // Unlock if needed
        if (SDL_MUSTLOCK(screen)) {
            SDL_UnlockSurface(screen);
        }

        mutex.post();
        
        // refresh the panel
        Refresh();
        //Update();
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
        stop();
        printf("frame gone\n");
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

    bool stop() {
        mutex.wait();
        wxsdl_stopped = true;
        mutex.post();
        return true;
    }
};

inline void SDLFrame::onFileExit(wxCommandEvent &) { 
    wxsdl_stopped = true;
    printf("On file exit\n");
    stop();
    mutex.wait();
    Close(); 
    mutex.post();
}
inline SDLPanel &SDLFrame::getPanel() { return *panel; }

IMPLEMENT_CLASS(SDLFrame, wxFrame)

BEGIN_EVENT_TABLE(SDLFrame, wxFrame)
    EVT_MENU(wxID_EXIT, SDLFrame::onFileExit)
    EVT_MENU(wxID_ABOUT, SDLFrame::onHelpAbout)
END_EVENT_TABLE()

SDLFrame::SDLFrame() {
    // Create the SDLFrame
    Create(0, IDF_FRAME, wxT("yarpview"), wxDefaultPosition,
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
    wxMessageBox(wxT("WxsdlWriter is based on the wx-sdl tutorial\nCopyright (C) 2005,2007 John Ratliff"),
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
    frame->SetClientSize(200, 200);
    frame->Centre();
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
    
    // generate an initial idle event to start things
    //wxIdleEvent event;
    //event.SetEventObject(&frame->getPanel());
    //frame->getPanel().AddPendingEvent(event);

    // start the main loop
    return wxApp::OnRun();
}

int SDLApp::OnExit() {
    printf("exiting\n");
    
    frame = NULL;

    printf("sdl shutdown\n");

    // cleanup SDL
    SDL_Quit();

    printf("standard exit\n");
    
    // return the standard exit code
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
    printf("WxsdlWriter finished\n");
    active = false;
}

bool WxsdlWriter::open(yarp::os::Searchable & config) {

    start();
    return true;
}
    
bool WxsdlWriter::close() {
    if (active) {
        active = false; 
        //wxGetApp().OnExit();
        wxGetApp().CleanUp();
    }
    if (!wxsdl_stopped) {
        stop();
    }
    return true;
}
  
bool WxsdlWriter::putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) {
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
    mutex.post();

    return result;
}
