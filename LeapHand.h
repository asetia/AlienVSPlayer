#pragma once
#ifndef LEAP_HAND_H
#define LEAP_HAND_H

#include "Shader.h"
#include "Util.h"

class LeapHand {
	Model handModel;
	mat4 handTransform;
	vec3 position;
	float scale;

	float leapScale;
	vec3 leapOffset;

public:
	LeapHand() : handModel("./hand.obj") {
		position = vec3(0, 0, 0);
		scale = 0.3;
		leapScale = 1.0f / 30.0f;
		leapOffset = vec3(0, -8, 0);
		update();
	}

	void update() {
		handTransform = glm::mat4(1.0f);
		handTransform = glm::rotate(glm::radians(90.0f), vec3(0, 1, 0));
		handTransform = glm::translate(handTransform, (glm::mat4(1.0f), vec3(0,0,0)));
		handTransform = glm::scale(handTransform, glm::vec3(scale, scale, scale));
	}


	void render(Shader& shader, const mat4 & projection, const mat4 & modelview) {
		if (showLeapHands) {
			position = leapHandPos * leapScale + leapOffset;
			update();

			std::cout << "Hand at " << glm::to_string(position) << std::endl;
			mat4 new_view = glm::translate(modelview, (glm::mat4(1.0f), position));

			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(new_view));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(handTransform));

			handModel.Draw(shader);
		}
	}
};

#endif // !LEAP_HAND_H
