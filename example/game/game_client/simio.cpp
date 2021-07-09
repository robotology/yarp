/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "simio.h"

#ifndef WIN32

int auto_refresh = 1;
int last_key = 0;
int key_stored = 0;

void newline()
{
    cputch ( '\n' );
}

void initconio()
{
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr,1);
}

void deinitconio()
{
    endwin();
}


void setautorefresh ( int refr )
{
    auto_refresh = refr;
}


void autorefresh()
{
    if ( auto_refresh )
        refresh();
}


void clrscr()
{
    clear();
    autorefresh();
}


void gotoxy ( int x, int y )
{
    move ( y, x );
    autorefresh();
}


void cputch ( int ch )
{
    addch ( ch );
    autorefresh();
}


int kbhit()
{
    if ( !key_stored )
        {
            last_key = getch();
            if ( last_key != ERR )
                {
                    key_stored = 1;
                }
        }

    return key_stored;
}

int waitkey()
{
    while ( !kbhit() )
        {
            // wait
        }
    key_stored = 0;

    return last_key;
}


#else

#include <conio.h>

void initconio()
{
    //textmode ( C4350 );
    //_setcursortype ( _NOCURSOR );
}


void newline()
{
    cputch ( '\r' );
    cputch ( '\n' );
}


void autorefresh() {
}


#include <windows.h>

void clrscr()
{
    COORD coordScreen = { 0, 0 }; /* here's where we'll home the cursor */
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
    DWORD dwConSize; /* number of character cells in the current buffer */

    /* get the output console handle */
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    /* get the number of character cells in the current buffer */
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    /* fill the entire screen with blanks */
    FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
                               dwConSize, coordScreen, &cCharsWritten);
    /* get the current text attribute */
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    /* now set the buffer's attributes accordingly */
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
                               dwConSize, coordScreen, &cCharsWritten);
    /* put the cursor at (0, 0) */
    SetConsoleCursorPosition(hConsole, coordScreen);
    return;
}

void gotoxy(int x, int y)
{
    COORD coordScreen = { x, y }; /* here's where we'll home the cursor */

    /* get the output console handle */
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    /* put the cursor at (0, 0) */
    SetConsoleCursorPosition(hConsole, coordScreen);
    return;
}

#endif
