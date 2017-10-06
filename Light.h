#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include "Util.h"
#include "Shader.h"

class Light {
public:
	static void Draw(Shader shader) {
		vec3 position(0.5f, 0.5f, 0.0f); // Approximately half a meter up in the air to the back right of the user.
		vec3 ambient(0.2f, 0.2f, 0.2f);
		vec3 diffuse(0.7f, 0.7f, 0.7f);
		vec3 specular(0.5f, 0.5f, 0.5f);

		glUniform3f(glGetUniformLocation(shader.Program, "light.position"),
			position.x,
			position.y,
			position.z);
		glUniform3f(glGetUniformLocation(shader.Program, "light.ambient"),
			ambient.r,
			ambient.g,
			ambient.b);
		glUniform3f(glGetUniformLocation(shader.Program, "light.diffuse"),
			diffuse.r,
			diffuse.g,
			diffuse.b);
		glUniform3f(glGetUniformLocation(shader.Program, "light.specular"),
			specular.r,
			specular.g,
			specular.b);
	}
};

#endif