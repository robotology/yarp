// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#pragma once

#include <stdio.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <GL/glut.h>

#define MAX_USERS 10
#define TOTAL_JOINTS 24

class GLWindow
{
public:
	GLWindow(void);
	GLWindow(int argc, char *argv[]);
	~GLWindow(void);
	void runGLWindow();
	void redraw();
	void setData(int userID, int userStatus, yarp::sig::Matrix *skeletonOrientation, double *confOrientation,
					   yarp::sig::Vector *skeletonPosition, double *confPosition);
private:
	static double _cameraPos[9];
	static int *_userStatus;
	static double _confOrientation[MAX_USERS][TOTAL_JOINTS], _confPosition[MAX_USERS][TOTAL_JOINTS];
	static yarp::sig::Matrix _skeletonOrientation[MAX_USERS][TOTAL_JOINTS];
	static yarp::sig::Vector _skeletonPosition[MAX_USERS][TOTAL_JOINTS];

	void initWindow(int argc, char *argv[], int width = 640, int height = 480);
	void initVars();
	static void simpleCube();
	static void drawJointPoints();
	static void drawJointOriAxis(int userID, int index);
	static void inputKey(int key, int x, int y);
	static void cameraRePos();
	static void displayDraw();
};
