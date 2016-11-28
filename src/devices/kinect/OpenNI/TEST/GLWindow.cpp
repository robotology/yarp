/*
* Copyright (C) 2011 Duarte Aragao
* Author: Duarte Aragao
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


#include "GLWindow.h"

double GLWindow::_cameraPos[9];
int *GLWindow::_userStatus;
yarp::sig::Matrix GLWindow::_skeletonOrientation[MAX_USERS][TOTAL_JOINTS];
yarp::sig::Vector GLWindow::_skeletonPosition[MAX_USERS][TOTAL_JOINTS];
double GLWindow::_confOrientation[MAX_USERS][TOTAL_JOINTS], GLWindow::_confPosition[MAX_USERS][TOTAL_JOINTS];

GLWindow::GLWindow(void)
{
}

GLWindow::GLWindow(int argc, char *argv[]){
	initVars();
	initWindow(argc, argv);
}

GLWindow::~GLWindow(void)
{
}


void GLWindow::initVars(){
	_userStatus = new int[MAX_USERS];
//	_skeletonOrientation = new yarp::sig::Matrix[MAX_USERS][TOTAL_JOINTS];
//	_skeletonPosition = new yarp::sig::Vector[MAX_USERS][TOTAL_JOINTS];
	for(int i = 0; i < 9; i++)
		_cameraPos[i] = 0;
}

void GLWindow::setData(int userID, int userStatus, yarp::sig::Matrix *skeletonOrientation, double *confOrientation,
					   yarp::sig::Vector *skeletonPosition, double *confPosition){
	_userStatus[userID] = userStatus;
	for(int i = 0; i < TOTAL_JOINTS; i++){
		_skeletonOrientation[userID][i] = skeletonOrientation[i];
		_skeletonPosition[userID][i] = skeletonPosition[i];
		_confPosition[userID][i] = confPosition[i];
		_confOrientation[userID][i] = confOrientation[i];
	}
}

void GLWindow::initWindow(int argc, char *argv[], int width, int height){
	glutInit(&argc,argv);
	glutInitWindowSize(width,height);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_DOUBLE);
	glutCreateWindow("GL_Skeleton");
	//set callbacks
	glutDisplayFunc(GLWindow::displayDraw);
	glutSpecialFunc(GLWindow::inputKey);
	//set projection
	double ratio = width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0,0,width,height);
	gluPerspective(45,ratio,1,20000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	_cameraPos[2] = 10000;
	//_cameraPos[5] = 1;
	_cameraPos[7] = 1;
	cameraRePos();

	//color settings
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
}

void GLWindow::runGLWindow(){
	glutMainLoop();
}

void GLWindow::redraw(){
	glutPostRedisplay();
}

void GLWindow::displayDraw(){
	glLoadIdentity();
	//set camera
	cameraRePos();
	//draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	drawJointPoints();
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void GLWindow::simpleCube(){
	glColor3f(0.9,0.0,0.0);
	glutWireSphere(2,10,10);
}


void GLWindow::cameraRePos(){
	gluLookAt(_cameraPos[0],_cameraPos[1],_cameraPos[2],_cameraPos[3],_cameraPos[4],_cameraPos[5],_cameraPos[6],_cameraPos[7],_cameraPos[8]);
}

void GLWindow::inputKey(int key, int x, int y){
	float step = 100;
	switch(key){
		case GLUT_KEY_LEFT:
			_cameraPos[0]-=step;
			_cameraPos[3]-=step;
			break;
		case GLUT_KEY_RIGHT:
			_cameraPos[0]+=step;
			_cameraPos[3]+=step;
			break;
		case GLUT_KEY_DOWN:
			_cameraPos[1]-=step;
			_cameraPos[4]-=step;
			break;
		case GLUT_KEY_UP:
			_cameraPos[1]+=step;
			_cameraPos[4]+=step;
			break;
		case GLUT_KEY_HOME:
			_cameraPos[2]+=step;
			_cameraPos[5]+=step;
			break;
		case GLUT_KEY_INSERT:
			_cameraPos[2]-=step;
			_cameraPos[5]-=step;
			break;
	}
	glutPostRedisplay();
}

void GLWindow::drawJointPoints(){
	glPointSize(6);
	glLineWidth(1);
	double x, y, z, posConf;
	for(int userID = 0; userID != MAX_USERS; userID++){
		if(_userStatus[userID]!=4) continue;
		yarp::sig::Vector *userSkeleton = _skeletonPosition[userID];
		double *userPositionConf = _confPosition[userID];
		for(int i = 0; i < TOTAL_JOINTS; i++){
			if(userSkeleton[i].size()!=3)continue;
			glPushMatrix();
			x = userSkeleton[i][0];
			y = userSkeleton[i][1];
			z = userSkeleton[i][2];
			posConf = userPositionConf[i];
			glTranslated(x,y,z);
			//position
			glColor3f(1*(1-posConf),1*posConf,1*posConf);
			glBegin(GL_POINTS);
			glVertex3f(0,0,0);
			glEnd();
			//orientation
			drawJointOriAxis(userID,i);
			glPopMatrix();
		}
	}
}

void GLWindow::drawJointOriAxis(int userID, int index){
	int length = 200;
	yarp::sig::Matrix oriMatrix = _skeletonOrientation[userID][index];
	if(oriMatrix.cols()!=3) return;
	double oriConf = _confOrientation[userID][index];

	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(oriMatrix[0][0]*length,oriMatrix[0][1]*length,oriMatrix[0][2]*length);
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(oriMatrix[1][0]*length,oriMatrix[1][1]*length,oriMatrix[1][2]*length);
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(oriMatrix[2][0]*length,oriMatrix[2][1]*length,oriMatrix[2][2]*length);
	glEnd();

}