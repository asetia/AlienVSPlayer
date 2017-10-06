#pragma once

#ifndef MESH_H
#define MESH_H

#include "Util.h"
#include "Shader.h"
//#include "Material.h"

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Material material;

	/*  Functions  */
	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->material = material;

		// Now that we have all the required data, set the vertex buffers and its attribute pointers.
		this->setupMesh();
	}

	// Render the mesh
	void Draw(Shader shader)
	{
		// Bind appropriate textures
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;

		// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
		glUniform3f(glGetUniformLocation(shader.Program, "material.ambient"),
			material.ambient.r,
			material.ambient.g,
			material.ambient.b);
		glUniform3f(glGetUniformLocation(shader.Program, "material.diffuse"),
			material.diffuse.r,
			material.diffuse.g,
			material.diffuse.b);
		glUniform3f(glGetUniformLocation(shader.Program, "material.specular"),
			material.specular.r,
			material.specular.g,
			material.specular.b);
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"),
			material.shininess);

		// Draw mesh
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh()
	{
		// Create buffers/arrays
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

		GLuint * index = this->indices.size() > 0 ? &this->indices[0] : 0;

		//WriteLog("index: %d \n", &this->indices[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), index, GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};
#endif // !MESH_H
