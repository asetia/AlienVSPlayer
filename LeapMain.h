#pragma once
#ifndef LEAP_MAIN_H
#define LEAP_MAIN_H

#include "Leap.h"
#include "Scene.h"
#include "Util.h"

#include <iostream>
#include <string.h>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

typedef struct {
	double x, y, z;
} XYZ;
typedef struct {
	unsigned char r, g, b, a;
} PIXELA;
typedef struct {
	double r, g, b;
} COLOUR;

#define ABS(x) (x < 0 ? -(x) : (x))
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define SIGN(x) (x < 0 ? (-1) : 1)
#define MODULUS(p) (sqrt(p.x*p.x + p.y*p.y + p.z*p.z))
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

typedef struct {
	XYZ vp;              /* View position           */
	XYZ vd;              /* View direction vector   */
	XYZ vu;              /* View up direction       */
	XYZ pr;              /* Point to rotate about   */
	double focallength;  /* Focal Length along vd   */
	double aperture;     /* Camera aperture         */
	double eyesep;       /* Eye separation          */
	int screenheight, screenwidth;
} CAMERA;

class App {
	std::shared_ptr<Scene> cubeScene;

public:
	App() {}


	void initGl() {
		glClearColor(0.2f, 0.3f, 0.8f, 0.0f);
		cubeScene = std::shared_ptr<Scene>(new Scene());
	}

	void renderScene(const glm::mat4 & projection, const glm::mat4 & modelview) {
		cubeScene->render(projection, modelview);
	}

};

class LeapListener : public Leap::Listener {
public:
	virtual void onConnect(const Leap::Controller&);
	virtual void onFrame(const Leap::Controller&);
};

void HandleDisplay(void);
void HandleKeyboard(unsigned char key, int x, int y);
void HandleReshape(int, int);
void Render(const glm::mat4 & projection, const glm::mat4 & modelview);
void CameraHome(int);
void CreateEnvironment(void);
void Normalise(XYZ *);
void RotateCamera(int, int, int);
void TranslateCamera(int, int);

inline vec3 toGlm(XYZ & x) {
	return vec3(x.x, x.y, x.z);
}

inline vec3 toGlm(Leap::Vector& v) {
	return vec3(v.x, v.y, v.z);
}

#endif // !LEAP_MEAN_H
