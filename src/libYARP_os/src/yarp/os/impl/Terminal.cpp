/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/Terminal.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformUnistd.h>

#include <cstdio>
#include <cstring>

#ifdef YARP_HAS_Libedit
#    include <editline/readline.h>
char* szLine = (char*)nullptr;
bool readlineEOF = false;
#endif // YARP_HAS_Libedit

bool yarp::os::impl::Terminal::EOFreached()
{
#ifdef YARP_HAS_Libedit
    if (yarp::os::impl::isatty(yarp::os::impl::fileno(stdin)) != 0) {
        return readlineEOF;
    }
#endif // YARP_HAS_Libedit
    return feof(stdin) != 0;
}

std::string yarp::os::impl::Terminal::getStdin()
{
    std::string txt;

#ifdef YARP_HAS_Libedit
    if (yarp::os::impl::isatty(yarp::os::impl::fileno(stdin)) != 0) {
        if (szLine != nullptr) {
            free(szLine);
            szLine = (char*)nullptr;
        }

        szLine = readline(">>");
        if ((szLine != nullptr) && (*szLine != 0)) {
            txt = szLine;
            add_history(szLine);
        } else if (szLine == nullptr) {
            readlineEOF = true;
        }
        return txt;
    }
#endif // YARP_HAS_Libedit

    bool done = false;
    char buf[2048];
    while (!done) {
        char* result = fgets(buf, sizeof(buf), stdin);
        if (result != nullptr) {
            for (unsigned int i = 0; i < strlen(buf); i++) {
                if (buf[i] == '\n') {
                    buf[i] = '\0';
                    done = true;
                    break;
                }
            }
            txt += buf;
        } else {
            done = true;
        }
    }
    return txt;
}

std::string yarp::os::impl::Terminal::readString(bool* eof)
{
    bool end = false;

    std::string txt;
    if (!EOFreached()) {
        txt = getStdin();
    }

    if (EOFreached() || (!txt.empty() && txt[0] < 32 && txt[0] != '\n' && txt[0] != '\r')) {
        end = true;
    }

    if (end) {
        txt = "";
    }

    if (eof != nullptr) {
        *eof = end;
    }
    return txt;
}
