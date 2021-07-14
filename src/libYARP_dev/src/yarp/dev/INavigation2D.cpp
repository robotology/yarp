/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/INavigation2D.h>

std::string yarp::dev::Nav2D::INavigation2DHelpers::statusToString(yarp::dev::Nav2D::NavigationStatusEnum status)
{
    if (status == navigation_status_idle) {
        return std::string("navigation_status_idle");
    } else if (status == navigation_status_moving) {
        return std::string("navigation_status_moving");
    } else if (status == navigation_status_waiting_obstacle) {
        return std::string("navigation_status_waiting_obstacle");
    } else if (status == navigation_status_goal_reached) {
        return std::string("navigation_status_goal_reached");
    } else if (status == navigation_status_aborted) {
        return std::string("navigation_status_aborted");
    } else if (status == navigation_status_failing) {
        return std::string("navigation_status_failing");
    } else if (status == navigation_status_paused) {
        return std::string("navigation_status_paused");
    } else if (status == navigation_status_preparing_before_move) {
        return std::string("navigation_status_preparing_before_move");
    } else if (status == navigation_status_thinking) {
        return std::string("navigation_status_thinking");
    } else if (status == navigation_status_error) {
        return std::string("navigation_status_error");
    }
    return std::string("navigation_status_error");
}

yarp::dev::Nav2D::NavigationStatusEnum yarp::dev::Nav2D::INavigation2DHelpers::stringToStatus(std::string s)
{
    yarp::dev::Nav2D::NavigationStatusEnum status;
    if (s == "navigation_status_idle") {
        status = navigation_status_idle;
    } else if (s == "navigation_status_moving") {
        status = navigation_status_moving;
    } else if (s == "navigation_status_waiting_obstacle") {
        status = navigation_status_waiting_obstacle;
    } else if (s == "navigation_status_goal_reached") {
        status = navigation_status_goal_reached;
    } else if (s == "navigation_status_aborted") {
        status = navigation_status_aborted;
    } else if (s == "navigation_status_failing") {
        status = navigation_status_failing;
    } else if (s == "navigation_status_paused") {
        status = navigation_status_paused;
    } else if (s == "navigation_status_preparing_before_move") {
        status = navigation_status_preparing_before_move;
    } else if (s == "navigation_status_thinking") {
        status = navigation_status_thinking;
    } else if (s == "navigation_status_error") {
        status = navigation_status_error;
    } else {
        status = navigation_status_error;
    }
    return status;
}
