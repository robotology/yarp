// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef SIMIO_INC
#define SIMIO_INC

extern void newline();

#ifndef WIN32
// UNIX version

//#include <ncurses.h>
extern "C" {
# include <curses.h>
}

extern void initconio();
extern void deinitconio();
extern void setautorefresh ( int refr );
extern void autorefresh();

extern int kbhit();
extern int waitkey();

extern void gotoxy ( int x, int y );
extern void clrscr();
extern void cputch ( int ch );

#define cprintf printw

#else
//MSDOS version

#include <conio.h>

extern void initconio();

#define deinitconio()
#define setautorefresh(refr)
#define refresh()

#define waitkey _getch
#define cputch _putch
//#define cputch putch // apparently this is deprecated
extern void autorefresh();
extern void clrscr();
extern void gotoxy(int x, int y);

#ifdef cprintf
#undef cprintf
#endif
#define cprintf _cprintf

#endif


#endif
