#pragma once
#ifndef RAY_H
#define RAY_H

#include "Util.h"


class Ray {
	Shader ray_shader;
	Model ray;
	int hand;
	MoleculeManager& mm;

public:
	Ray(MoleculeManager& mm, int handIndex) : ray_shader((SRC_DIR + "ray.vs").c_str(), (SRC_DIR + "ray.frag").c_str()),
		ray("./ray/ray.obj"),
		hand(handIndex),
		mm(mm) {}

	Molecule* render(const mat4 & projection, const mat4 & modelview) {
		ray_shader.Use();
		glUniformMatrix4fv(glGetUniformLocation(ray_shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(ray_shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(modelview));

		bool isTriggered = indexTrigger[hand] > 0.9f;
		vec4 green(0.14f, 0.9f, 0.14f, 0.3f);
		vec4 red(0.9f, 0.14f, 0.14f, 0.3f);
		vec4 chosenColor = isTriggered ? red : green;

		glUniform4f(glGetUniformLocation(ray_shader.Program, "desiredColor"),
			chosenColor.r,
			chosenColor.g,
			chosenColor.b,
			chosenColor.a);

		float length = 10.0f;

		Molecule* hit_molecule = nullptr;
		float hit_dist;
		raycast(&hit_molecule, &hit_dist);

		if (hit_dist > 2.1f) {
			hit_molecule = nullptr;
		}

		// Epic pew pew laser. RIP
		length = 0.1f + indexTrigger[hand] * 2.0f;

		glm::mat4 model = handMat[hand];
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		model = glm::scale(model, glm::vec3(0.7f, length, 0.7f)); // It's a bit too big for our scene, so scale it down

		glUniformMatrix4fv(glGetUniformLocation(ray_shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		ray.Draw(ray_shader);

		if (isTriggered) return hit_molecule;

		return nullptr;
	}

	void raycast(Molecule** hit_molecule, float* hit_dist) {
		*hit_molecule = nullptr;
		//WriteLog("START");
		float closest_dist = 9999999.0f;
		vec3 O = handPos[hand];
		//WriteLog("O: %f, %f, %f\n", O.x, O.y, O.z);
		vec3 D = handMat[hand] * vec4(0, 0, -1, 0);
		//WriteLog("D: %f, %f, %f\n", D.x, D.y, D.z);
		float r = 0.3f;
		for (int i = 0; i < mm.molecules.size(); i++) {
			if (!mm.molecules[i]->isCo2) continue;
			vec3 C = mm.molecules[i]->position;
			//WriteLog("C: %f, %f, %f\n", C.x, C.y, C.z);
			vec3 L = C - O;
			//WriteLog("L: %f, %f, %f\n", L.x, L.y, L.z);
			float tca = glm::dot(L, glm::normalize(D));
			if (tca < 0) {
				continue;
			}
			float d2 = glm::dot(L, L) - tca * tca;
			//WriteLog("mL: %f, tca: %f\n", glm::length(L), tca);
			//WriteLog("d: %f\n", glm::sqrt(d2));
			if (d2 < 0 || d2 > r * r) {
				continue;
			}

			// We have a hit! Now calculate the point of hit.
			float thc = 42 * 0 + glm::sqrt(r * r - d2);
			float mP = tca - thc;
			vec3 P = O + mP * D;


			if (mP < closest_dist) {
				closest_dist = mP;
				*hit_dist = mP;
				*hit_molecule = mm.molecules[i];
			}
		}
	}
};
#endif