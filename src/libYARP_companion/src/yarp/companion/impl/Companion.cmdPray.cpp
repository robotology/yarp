/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/conf/environment.h>

#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <algorithm>
#include <cstdarg>

#if defined(__unix__)
#include <unistd.h>
#include <termios.h>
#elif defined(_MSC_VER)
#include <conio.h>
#endif


using yarp::companion::impl::Companion;
using yarp::os::NetworkBase;


// yarp pray output should not be using yarp logging utilities
int Companion::cmdPray(int argc, char *argv[])
{
    auto cmdPray_usage = [](){
        yCInfo(COMPANION, "Usage:");
        yCInfo(COMPANION, "yarp pray [port]");
    };

    auto cmdPray_getch = []() {
#if defined(__unix__)
        char buf = 0;
        struct termios old;
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (::read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
#elif defined(_MSC_VER)
        return static_cast<char>(_getch());
#else
        return static_cast<char>(getchar());
#endif
    };

    bool palindrome = false;

    auto cmdPray_makePalindrome = [](std::string s) {
        bool newline = false;
        if (s.find('\n') != std::string::npos) {
            newline = true;
            s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        }
        std::string sr = s;
        std::reverse(sr.begin(), sr.end());

        sr = sr.substr(1);
        return s + sr + (newline ? "\n" : "");
    };

    auto cmdPray_printf = [&](const char* format, ...) YARP_ATTRIBUTE_FORMAT(printf, 2, 3)
    {
        va_list args;
        va_start(args, format);
        if (!palindrome) {
            printf(format, args);
        } else {
            constexpr size_t buf_size = 1024;
            char buffer[buf_size];
            vsnprintf(buffer, buf_size, format, args);
            va_end(args);
            auto s = cmdPray_makePalindrome(buffer);
            printf("%s", s.c_str());
        }
    };


    if (argc > 3) {
        cmdPray_usage();
        return 1;
    }

    std::string name;
    bool local = true;
    std::string state;
    if (argc == 3) {
        name = argv[2];
        if (!NetworkBase::exists(name, true)) {
            // Remove initial "/"
            while (name[0] == '/') {
                name = name.substr(1);
            }
            // Keep only the first part of the port name
            auto i = name.find('/');
            if (i != std::string::npos) {
                name = name.substr(0, i);
            }
            state = "not available";
        } else {
            local = false;
        }
    } else {
        if (std::string(argv[0]) == "yarp" && std::string(argv[1]) == "pray") {
            palindrome = true;
            cmdPray_printf("Entering palindrome mode.\n");
            state = "palindrome";
        } else {
            state = "displeased";
        }
        bool found = false;
        name = yarp::conf::environment::get_string("YARP_ROBOT_NAME", &found);
        if (!found) {
            name = "YARPino";
        }
    }

    char c;
    printf("Are you sure you want to pray? [yn] (n) ");
    fflush(stdout);

    do {
        c = cmdPray_getch();
    } while (c != 'y' && c != 'n');
    printf("\n");

    if (c == 'n') {
        return 0;
    }

    if (local) {
        printf("You begin praying to %s.  You finish your prayer.  You feel that %s is %s.\n", name.c_str(), name.c_str(), state.c_str());
        return 0;
    }

    yarp::os::Bottle cmd;
    yarp::os::Bottle reply;
    cmd.addVocab32('p', 'r', 'a', 'y');
    yarp::os::NetworkBase::write(name, cmd, reply, true, true);

    bool first = true;
    for (size_t i = 0; i < reply.size(); ++i) {
        if (!first) {
            printf("  ");
        }
        first = false;
        printf("%s", reply.get(i).asString().c_str());
    }
    printf("\n");
    return 0;
}
