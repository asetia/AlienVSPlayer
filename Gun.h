#pragma once
#ifndef GUN_H
#define GUN_H

#include "Util.h"

class Gun {
	Shader gun_shader;
	Model gun;
	int hand;

public:
	Gun(int handIndex) : gun_shader((SRC_DIR + "default.vs").c_str(), (SRC_DIR + "default.frag").c_str()),
		gun("./gun/gun.obj"),
		hand(handIndex) {}

	void render(const mat4 & projection, const mat4 & modelview) {
		gun_shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(gun_shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(gun_shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(modelview));

		glm::mat4 model = handMat[hand];
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(0.0f, -0.065f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f)); // It's a bit too big for our scene, so scale it down

		glUniformMatrix4fv(glGetUniformLocation(gun_shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		gun.Draw(gun_shader);
	}

};

#endif // !GUN_H

