#pragma once

#ifndef UTIL_H
#define UTIL_H

#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/mesh.h>
#include<assimp/postprocess.h> 

#define FAIL(X) throw std::runtime_error(X)

static std::string SRC_DIR = "C:/Users/r2tao/Desktop/CSE190Project4/";

template<class... Args>
inline void WriteLog(Args&&... args) {
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), args...);
	OutputDebugString(buffer);
}

struct Vertex { 
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	std::string type;
	aiString path;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

// Global Variables
glm::vec3 handPos[2];
glm::mat4 handMat[2];

float indexTrigger[2];
bool buttonA;
bool buttonB;
vec3 leapHandPos;
bool showLeapHands = false;

//! Load a ppm file from disk.
// @input filename The location of the PPM file.  If the file is not found, an error message
//		will be printed and this function will return 0
// @input width This will be modified to contain the width of the loaded image, or 0 if file not found
// @input height This will be modified to contain the height of the loaded image, or 0 if file not found
//
// @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured

unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return 0;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;

		return 0;
	}
	return rawData;
}


#endif // !UTIL_H
