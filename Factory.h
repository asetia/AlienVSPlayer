#pragma once
#ifndef FACTORY_H
#define FACTORY_H

#include "Model.h"
#include "Shader.h"
#include "Util.h"

class Factory {

public:
	Model m;

	Factory() : m("./factory1/factory1.obj") {

	}

	void render(Shader shader, const mat4 & projection, const mat4 & modelview) {
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f)); // It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		m.Draw(shader);
	}
};
#endif // !FACTORY_H
