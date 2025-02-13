/*
 * SPDX-FileCopyrightText: 2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CONTROL_CLOCK
#define CONTROL_CLOCK

class ControlClock
{
private:
    bool running = false;
    double startTime = 0.0;
    double elapsedTime = 0.0;
public:
    double getElapsedTime();
    void startTimer();
    void pauseTimer();
    void resetTimer();
};

#endif
