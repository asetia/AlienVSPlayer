#pragma once
#define _CRT_SECURE_NO_DEPRECATE
// may need #include "stdafx.h" in visual studio
//#include "stdafx.h"
#include "ServerGame.h"
#include "ClientGame.h"
// used for multi-threading
#include <process.h>

void serverLoop(void *);
void clientLoop(void);

ServerGame * server;
ClientGame * client;

#include "LeapMain.h"

using namespace Leap;

/* Glasses filter types */
#define REDBLUE  1
#define REDGREEN 2
#define REDCYAN  3
#define BLUERED  4
#define GREENRED 5
#define CYANRED  6
int glassestype = REDCYAN;

#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322
#define ESC 27

App app;

CAMERA camera;
bool fullscreen = false;
int debug = FALSE;

XYZ origin = { 0.0,0.0,0.0 };

int main(int argc, char** argv) {
	LeapListener listener;
	Controller controller;
	controller.addListener(listener);
	
	// initialize the client
	//client = new ClientGame();

	/* Set things (glut) up */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);

	/* Create the window and handlers */
	glutCreateWindow("Aliens vs. Player");
	camera.screenwidth = 1920;
	camera.screenheight = 1080;
	glutReshapeWindow(camera.screenwidth, camera.screenheight);
	if (fullscreen)
		glutFullScreen();
	glutIdleFunc(HandleDisplay);

	glewExperimental = GL_TRUE;
	if (0 != glewInit()) {
		FAIL("Failed to initialize GLEW");
	}
	glGetError();

	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// http://paulbourke.net/stereographics/anaglyph/anaglyph.c

	/* Set things (glut) up */

	CreateEnvironment();
	CameraHome(0);

	app.initGl();

	// Keep this process running until Enter is pressed
	//std::cout << "Press Enter to quit..." << std::endl;
	//std::cin.get();

	glutMainLoop();
	glfwTerminate();
	controller.removeListener(listener);
	return(0);
}
void LeapListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
}
void LeapListener::onFrame(const Controller& controller) {
	const Frame frame = controller.frame();
	if (frame.hands().count() > 0) {
		showLeapHands = true;
		leapHandPos = toGlm(frame.hands()[0].palmPosition());
	}
	else {
		showLeapHands = true;
	}

}

/*
This is the basic display callback routine
It creates the geometry, lighting, and viewing position
*/
void HandleDisplay(void)
{
	int i, j;
	XYZ r;
	double dist, ratio, radians, scale, wd2, ndfl;
	double left, right, top, bottom, near_ = 0.1, far_ = 10000;

	/* Clip to avoid extreme stereo */
	near_ = camera.focallength / 5;

	/* Derive the the "right" vector */
	CROSSPROD(camera.vd, camera.vu, r);
	Normalise(&r);
	r.x *= camera.eyesep / 2.0;
	r.y *= camera.eyesep / 2.0;
	r.z *= camera.eyesep / 2.0;

	/* Misc stuff */
	ratio = camera.screenwidth / (double)camera.screenheight;
	radians = DTOR * camera.aperture / 2;
	wd2 = near_ * tan(radians);
	ndfl = near_ / camera.focallength;

	/* Set the buffer for writing and reading */
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	/* Clear things */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_ACCUM_BUFFER_BIT); /* Not strictly necessary */

	glViewport(0, 0, camera.screenwidth, camera.screenheight);

	/* Left eye filter */
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	switch (glassestype) {
	case REDBLUE:
	case REDGREEN:
	case REDCYAN:
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		break;
	case BLUERED:
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
		break;
	case GREENRED:
		glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
		break;
	case CYANRED:
		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		break;
	}

	/* Create the projection */
	left = -ratio * wd2 + 0.5 * camera.eyesep * ndfl;
	right = ratio * wd2 + 0.5 * camera.eyesep * ndfl;
	top = wd2;
	bottom = -wd2;
	mat4 lProjection = glm::frustum(left, right, bottom, top, near_, far_);

	/* Create the model */
	mat4 lModelview = lookAt(vec3(camera.vp.x - r.x, camera.vp.y - r.y, camera.vp.z - r.z),
		vec3(camera.vp.x - r.x + camera.vd.x,
		camera.vp.y - r.y + camera.vd.y,
		camera.vp.z - r.z + camera.vd.z),
		vec3(camera.vu.x, camera.vu.y, camera.vu.z));
	Render(lProjection, lModelview);
	glFlush();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	/* Write over the accumulation buffer */
	glAccum(GL_LOAD, 1.0); /* Could also use glAccum(GL_ACCUM,1.0); */

	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Right eye filter */
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	switch (glassestype) {
	case REDBLUE:
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
		break;
	case REDGREEN:
		glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
		break;
	case REDCYAN:
		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		break;
	case BLUERED:
	case GREENRED:
	case CYANRED:
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		break;
	}

	/* The projection */
	left = -ratio * wd2 - 0.5 * camera.eyesep * ndfl;
	right = ratio * wd2 - 0.5 * camera.eyesep * ndfl;
	top = wd2;
	bottom = -wd2;
	mat4 rProjection = glm::frustum(left, right, bottom, top, near_, far_);
	
	mat4 rModelview = lookAt(vec3(camera.vp.x + r.x, camera.vp.y + r.y, camera.vp.z + r.z),
		vec3(camera.vp.x + r.x + camera.vd.x,
		camera.vp.y + r.y + camera.vd.y,
		camera.vp.z + r.z + camera.vd.z),
		vec3(camera.vu.x, camera.vu.y, camera.vu.z));
	Render(rProjection, rModelview);
	glFlush();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	//std::cout << glm::to_string(lModelview) << std::endl;

	/* Addin the new image and copy the result back */
	glAccum(GL_ACCUM, 1.0);
	glAccum(GL_RETURN, 1.0);

	/* Let's look at it */
	glutSwapBuffers();
}

void Render(const glm::mat4 & projection, const glm::mat4 & modelview) {

	app.renderScene(projection, modelview);
	//client->update();
}


/*
Move the camera to the home position
Or to a predefined stereo configuration
The model is assumed to be in a 10x10x10 cube
Centered at the origin
*/
void CameraHome(int mode)
{
	camera.aperture = 60;
	camera.pr = origin;

	camera.vd.x = 0;
	camera.vd.y = 0;
	camera.vd.z = -1;

	camera.vu.x = 0;
	camera.vu.y = 3;
	camera.vu.z = 0;

	camera.vp.x = 0;
	camera.vp.y = 0;
	camera.vp.z = 12;
	
	RotateCamera(0, 20, 0);

	switch (mode) {
	case 0:
	case 2:
	case 4:
		camera.focallength = 10;
		break;
	case 1:
		camera.focallength = 5;
		break;
	case 3:
		camera.focallength = 15;
		break;
	}

	/* Non stressful stereo setting */
	camera.eyesep = camera.focallength / 30.0;
	if (mode == 4)
		camera.eyesep = 0;
}

/*
This is where global OpenGL/GLUT settings are made,
that is, things that will not change in time
*/
void CreateEnvironment(void)
{
	int num[2];

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_DITHER);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);    /* Not necessary but for bug in PS350 driver */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.0);
	glPointSize(1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace(GL_CW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearAccum(0.0, 0.0, 0.0, 0.0);   /* The default */
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*
	In case you want to check what the colour depth of your
	accumulation buffer is, hopefully it is 16 bits
	*/
	if (debug) {
		glGetIntegerv(GL_ACCUM_RED_BITS, num);
		fprintf(stderr, "Red bits: %d\n", num[0]);
		glGetIntegerv(GL_ACCUM_GREEN_BITS, num);
		fprintf(stderr, "Green bits: %d\n", num[0]);
		glGetIntegerv(GL_ACCUM_BLUE_BITS, num);
		fprintf(stderr, "Blue bits: %d\n", num[0]);
		glGetIntegerv(GL_ACCUM_ALPHA_BITS, num);
		fprintf(stderr, "Alpha bits: %d\n", num[0]);
	}


}


/*
Rotate (ix,iy) or roll (iz) the camera about the focal point
ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
Correctly updating all camera attributes
*/
void RotateCamera(int ix, int iy, int iz)
{
	XYZ vp, vu, vd;
	XYZ right;
	XYZ newvp, newr;
	double radius, dd, radians;
	double dx, dy, dz;

	vu = camera.vu;
	Normalise(&vu);
	vp = camera.vp;
	vd = camera.vd;
	Normalise(&vd);
	CROSSPROD(vd, vu, right);
	Normalise(&right);
	radians = 1 * PI / 180.0;

	/* Handle the roll */
	if (iz != 0) {
		camera.vu.x += iz * right.x * radians;
		camera.vu.y += iz * right.y * radians;
		camera.vu.z += iz * right.z * radians;
		Normalise(&camera.vu);
		return;
	}

	/* Distance from the rotate point */
	dx = camera.vp.x - camera.pr.x;
	dy = camera.vp.y - camera.pr.y;
	dz = camera.vp.z - camera.pr.z;
	radius = sqrt(dx*dx + dy*dy + dz*dz);

	/* Determine the new view point */
	dd = radius * radians;
	newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
	newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
	newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
	Normalise(&newvp);
	camera.vp.x = camera.pr.x + radius * newvp.x;
	camera.vp.y = camera.pr.y + radius * newvp.y;
	camera.vp.z = camera.pr.z + radius * newvp.z;

	/* Determine the new right vector */
	newr.x = camera.vp.x + right.x - camera.pr.x;
	newr.y = camera.vp.y + right.y - camera.pr.y;
	newr.z = camera.vp.z + right.z - camera.pr.z;
	Normalise(&newr);
	newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
	newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
	newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

	camera.vd.x = camera.pr.x - camera.vp.x;
	camera.vd.y = camera.pr.y - camera.vp.y;
	camera.vd.z = camera.pr.z - camera.vp.z;
	Normalise(&camera.vd);

	/* Determine the new up vector */
	CROSSPROD(newr, camera.vd, camera.vu);
	Normalise(&camera.vu);
}

/*
Normalise a vector
*/
void Normalise(XYZ *p)
{
	double length;

	length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
	if (length != 0) {
		p->x /= length;
		p->y /= length;
		p->z /= length;
	}
	else {
		p->x = 0;
		p->y = 0;
		p->z = 0;
	}
}

