#pragma once

#ifndef MOLECULE_H
#define MOLECULE_H

class Molecule {

public:
	Model * m;
	bool isCo2;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 axis;
	float spin;
	glm::mat4 model;
	glm::mat4 rotate;

	Molecule(Model * _m) {
		m = _m;
		isCo2 = true;

		double pi = 3.1415926535897;
		float angle = findRand(0.0f, 1.0f) * pi * 2;
		float x = cos(angle) * 5.0f;
		float y = sin(angle) * 5.0f;
		position = glm::vec3(x, -1.0f, y);

		//position = glm::vec3(0.0f, -0.5f, -1.0f);
		//velocity = glm::vec3(findRand(-1.0f, 1.0f) + 0.2f, findRand(0.0f, 1.0f) + 0.2f, findRand(-1.0f, 1.0f) + 0.2f);
		velocity = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f) - position);
		axis = glm::vec3(findRand(-1.0f, 1.0f), findRand(-1.0f, 1.0f), findRand(-1.0f, 1.0f));
		spin = (float)(findRand(-1.0f, 1.0f));
		model = glm::mat4(1.0f);

		model = glm::translate(model, (glm::mat4(1.0f), position));
		//model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		//model = glm::rotate(model, findRand(0.0f, 360.0f), glm::vec3(0, 1, 0));
	}

	void update() {
		position += velocity * 0.001f;
		//rotate = rotate * glm::rotate(glm::mat4(1.0f), spin / 180.0f * glm::pi<float>(), axis);
		model = glm::mat4(1.0f);
		model = glm::translate(model, (glm::mat4(1.0f), position));
		//model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));

		if (position.x < 0.2f && position.x > -0.2f) {
			//std::cout << "game over" << std::endl;
		}

		if (position.z < 0.2f && position.z > -0.2f) {
			//std::cout << "game over" << std::endl;
		}
	}

	float findRand(float min, float max) {
		return min + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / (max - min)));
	}

	void randomizePosition() {
		position = glm::vec3(findRand(-1.0f, 1.0f), findRand(-0.5f, 1.5f), findRand(-2.0f, 0.0f));
	}

	void render(Shader shader, const mat4 & projection, const mat4 & modelview) {
		/*glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f)); // It's a bit too big for our scene, so scale it down
		*/
		update();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		m->Draw(shader);
	}
};
#endif // !MOLECULE_H
