#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include "Model.h"
#include "Util.h"

class Terrain {
	Model terrainModel;
	mat4 terrainTransform;
	vec3 position;
	float scale;

public:
	Terrain() : terrainModel("./desert.obj") {
		position = vec3(0, -20, 0);
		scale = 0;
		update();
	}

	void update() {
		terrainTransform = glm::mat4(1.0f);
		terrainTransform = glm::translate(terrainTransform, (glm::mat4(1.0f), vec3(0, 0, 0)));
		terrainTransform = glm::scale(terrainTransform, glm::vec3(scale, scale, scale));
	}


	void render(Shader& shader, const mat4 & projection, const mat4 & modelview) {
		mat4 new_view = glm::translate(modelview, (glm::mat4(1.0f), position));

		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(new_view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(terrainTransform));

		terrainModel.Draw(shader);
	}
};

#endif