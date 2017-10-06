#pragma once
#ifndef CUBE_H
#define CUBE_H

#include "Util.h"

static const char * VERTEX_SHADER = R"SHADER(
#version 410 core
uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);
uniform mat4 InstanceTransform = mat4(1);
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 vertexUV; //190
out vec3 vertNormal;
out vec2 UV; //190
void main(void) {
	UV = vertexUV; //190
   mat4 ViewXfm = CameraMatrix * InstanceTransform;
   //mat4 ViewXfm = CameraMatrix;
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}
)SHADER";

static const char * FRAGMENT_SHADER = R"SHADER(
#version 410 core
in vec2 UV; //190
out vec4 fragColor;
out vec3 color; //190
uniform sampler2D myTextureSampler; //190
void main(void) {
	color = texture( myTextureSampler, UV).rgb;
    fragColor = vec4(color, 1.0);
}
)SHADER";

// Referenced from https://github.com/HT413/CSE167StarterCode2/blob/master/Cube.cpp
class Cube
{
	Shader shader;
	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;
	GLuint uv_ID;
	GLuint texture_ID, textureSpl_handle;

public:
	float scale;
	vec3 pos;
	vec3 rot_axis;
	float rot_angle; // radians
	Cube(const char* texture_ppm) : shader((SRC_DIR + "cube.vs").c_str(), (SRC_DIR + "cube.frag").c_str()),
		scale(10.0f), pos(vec3(0, -0.6, 0)), rot_axis(vec3(1, 0, 0)), rot_angle(0.0f) {
		// Cube indices
		// Slides are wrong, it goes from 0-7, not 0-8
		//    +y               5_______6     
		//    ^               /       /|    
		//    |              /1_____2/ |   
		//    +---> +x       | .     | |      
		//   /               | 4     | /7   
		//  v                |_______|/       
		// +z                0      3                 

		// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
		// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
		// This just looks nicer since it's easy to tell what coordinates/indices belong where.
		const GLfloat vertices[36][3] = {
			// Front face
			{ -0.5, -0.5,  0.5 }, // 0
			{ -0.5,  0.5,  0.5 }, // 1
			{ 0.5,  0.5,  0.5 }, // 2
			{ 0.5,  0.5,  0.5 }, // 2
			{ 0.5, -0.5,  0.5 }, // 3
			{ -0.5, -0.5,  0.5 }, // 0
								  // Top face
			{ -0.5,  0.5,  0.5 },  // 1
			{ -0.5,  0.5, -0.5 },  // 5
			{ 0.5,  0.5, -0.5 },  // 6
			{ 0.5,  0.5, -0.5 },  // 6
			{ 0.5,  0.5,  0.5 },  // 2
			{ -0.5,  0.5,  0.5 },  // 1
								   // Back face
			{ 0.5, -0.5, -0.5 }, // 7
			{ 0.5,  0.5, -0.5 }, // 6
			{ -0.5,  0.5, -0.5 }, // 5
			{ -0.5,  0.5, -0.5 }, // 5
			{ -0.5, -0.5, -0.5 }, // 4
			{ 0.5, -0.5, -0.5 }, // 7
								 // Bottom face
			{ -0.5, -0.5, -0.5 }, // 4
			{ -0.5, -0.5,  0.5 }, // 0
			{ 0.5, -0.5,  0.5 }, // 3
			{ 0.5, -0.5,  0.5 }, // 3
			{ 0.5, -0.5, -0.5 }, // 7
			{ -0.5, -0.5, -0.5 }, // 4
								  // Left face
			{ -0.5, -0.5, -0.5 }, // 4
			{ -0.5,  0.5, -0.5 }, // 5
			{ -0.5,  0.5,  0.5 }, // 1
			{ -0.5,  0.5,  0.5 }, // 1
			{ -0.5, -0.5,  0.5 }, // 0
			{ -0.5, -0.5, -0.5 }, // 4
								  // Right face
			{ 0.5, -0.5,  0.5 }, // 3
			{ 0.5,  0.5,  0.5 }, // 2
			{ 0.5,  0.5, -0.5 }, // 6
			{ 0.5,  0.5, -0.5 }, // 6
			{ 0.5, -0.5, -0.5 }, // 7
			{ 0.5, -0.5,  0.5 }  // 3
		};

		// CSE190 uvs
		const GLfloat uvs[72] = {
			// Front face
			0.0f, 1.0f, // 0 BOTTOM-LEFT
			0.0f, 0.0f, // 1 TOP-LEFT
			1.0f, 0.0f, // 2 TOP-RIGHT
			1.0f, 0.0f, // 2 TOP-RIGHT
			1.0f, 1.0f, // 3 BOTTOM-RIGHT
			0.0f, 1.0f, // 0 BOTTOM-LEFT
						// Top face
						0.0f, 1.0f, // 1
						0.0f, 0.0f, // 5
						1.0f, 0.0f, // 6
						1.0f, 0.0f, // 6
						1.0f, 1.0f, // 2
						0.0f, 1.0f, // 1
									// Back face
									0.0f, 1.0f, // 7
									0.0f, 0.0f, // 6
									1.0f, 0.0f, // 5
									1.0f, 0.0f, // 5
									1.0f, 1.0f, // 4
									0.0f, 1.0f, // 7
												// Bottom face
												0.0f, 1.0f, // 4
												0.0f, 0.0f, // 0
												1.0f, 0.0f, // 3
												1.0f, 0.0f, // 3
												1.0f, 1.0f, // 7
												0.0f, 1.0f, // 4
															// Left Face
															0.0f, 1.0f, // 4
															0.0f, 0.0f, // 5
															1.0f, 0.0f, // 1
															1.0f, 0.0f, // 1
															1.0f, 1.0f, // 0
															0.0f, 1.0f, // 4
																		// Right Face
																		0.0f, 1.0f, // 3
																		0.0f, 0.0f, // 2
																		1.0f, 0.0f, // 6
																		1.0f, 0.0f, // 6
																		1.0f, 1.0f, // 7
																		0.0f, 1.0f, // 3
		};

		// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
		// Consider the VAO as a container for all your buffers.
		glBindVertexArray(VAO);

		// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
		// you want to draw, such as vertices, normals, colors, etc.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
		// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Enable the usage of layout location 0 (check the vertex shader to see what this is)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
			3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
			GL_FLOAT, // What type these components are
			GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
			3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
			(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

						 // CSE190: Add textureUV to VAO
		glGenBuffers(1, &uv_ID);
		glBindBuffer(GL_ARRAY_BUFFER, uv_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,  // Tightly packed, no stride. Same thing as "2"
			0); // No offset

				// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Unbind the VAO now so we don't accidentally tamper with it.
		// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
		glBindVertexArray(0);

		// Set up texture
		unsigned char * _data;
		int _width, _height;
		// CSE190: Load PPM
		_data = loadPPM(texture_ppm, _width, _height);
		glGenTextures(1, &texture_ID);
		glBindTexture(GL_TEXTURE_2D, texture_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, _data);
		// Done with data, delete it.
		delete _data;
		// Setup filtering (explained on slide 13)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// Get a handle for our "myTextureSampler" uniform, and an ID for uv coordinates.
		textureSpl_handle = glGetUniformLocation(shader.Program, "myTextureSampler");		// Oglplus equivalent of above:
																							//GLuint textureSpl_handle = GetGLLocation(UniformLoc(prog, "myTextureSampler"));

	}

	~Cube() {
		// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
		// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void render(const mat4 & projection, const mat4 & modelview) {
		shader.Use();

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "CameraMatrix"), 1, GL_FALSE, glm::value_ptr(modelview));
		mat4 scaleMatrix = glm::scale(vec3(scale, scale, scale));
		mat4 translateMatrix = glm::translate(pos);
		mat4 rotationMatrix = glm::rotate(rot_angle, rot_axis);
		mat4 cubeTransformMatrix = scaleMatrix * translateMatrix *  rotationMatrix;
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "InstanceTransform"), 1, GL_FALSE, glm::value_ptr(cubeTransformMatrix));

		// Now draw the cube. We simply need to bind the VAO associated with it.
		glBindVertexArray(VAO);

		// CSE190
		// Bind texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_ID);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(textureSpl_handle, 0);
		// Draw arrays.
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
		glBindVertexArray(0);
	}
};

#endif // !CUBE_H