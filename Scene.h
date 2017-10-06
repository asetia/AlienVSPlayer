#ifndef SCENE_H
#define SCENE_H

#include <ctime>

#include "Shader.h"
#include "Model.h"
#include "Factory.h"
#include "Molecule.h"
#include "MoleculeManager.h"
#include "Ray.h"
#include "Light.h"
#include "Gun.h"
#include "Cube.h"
#include "LeapHand.h"
#include "Terrain.h"

// a class for encapsulating building and rendering an RGB cube
struct Scene {

	Shader shader;
	Model factory;
	Model co2;
	Model o2;
	LeapHand hand;
	Terrain terrain;

	int co2Count;
	int moleculeCount;

	Factory f;
	MoleculeManager mm;
	Ray leftRay;
	Ray rightRay;
	Gun leftGun;
	Gun rightGun;
	time_t start;

	Cube floor;
	Cube calibration_cube;

	// VBOs for the cube's vertices and normals

	const unsigned int GRID_SIZE{ 5 };

public:
	Scene() : shader((SRC_DIR + "default.vs").c_str(), (SRC_DIR + "default.frag").c_str()),
		factory("./factory1/factory1.obj"),
		co2("./co2/co2.obj"),
		o2("./o2/o2.obj"),
		leftRay(mm, 0),
		rightRay(mm, 1),
		leftGun(0),
		rightGun(1),
		floor("./Vr_test_pattern/vr_test_pattern.ppm"),
		calibration_cube("./Vr_test_pattern/vr_test_pattern2.ppm") {

		start = time(0);
		calibration_cube.scale = 1.0f;
		calibration_cube.pos = vec3(3.0f, -0.5f, -4.0f);
		co2Count = 0;
		moleculeCount = 0;
	}

	void displayFactory(const mat4 & projection, const mat4 & modelview, Shader shader, Model factory) {
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f)); // It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		factory.Draw(shader);
	}

	void displayMolecule(const mat4 & projection, const mat4 & modelview, Shader shader, Model molecule) {
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -1.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f)); // It's a bit too big for our scene, so scale it down
		model = glm::rotate(model, 90.0f, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		molecule.Draw(shader);
		co2Count++;
		moleculeCount++;
	}

	void render(const mat4 & projection, const mat4 & modelview) {

		shader.Use();   // <-- Don't forget this one!
						// Transformation matrices
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(modelview));

		// Set view pos
		vec3 viewPos = glm::inverse(modelview)[3]; // Top 3 values of right-most column of headpose.

		glUniform3f(glGetUniformLocation(shader.Program, "viewPos"),
			viewPos.x,
			viewPos.y,
			viewPos.z);

		// Draw the light
		Light::Draw(shader);

		// Draw the loaded model
		//displayFactory(projection, modelview, shader, factory);

		//f.render(shader, projection, modelview);
		

	    mm.renderMolecules(shader, projection, modelview);

		if (mm.count <= 0) {
			mm.gameWon();
		}

		if (buttonA && mm.isGameOver) {
			mm.restart();
		}

		if (difftime(time(0), start) > 2) {
			start = time(0);
			mm.addMolecule();
		}
		terrain.render(shader, projection, modelview);
		hand.render(shader, projection, modelview);

		Molecule* leftMolecule = leftRay.render(projection, modelview);
		Molecule* rightMolecule = rightRay.render(projection, modelview);

		leftGun.render(projection, modelview);
		rightGun.render(projection, modelview);

		floor.render(projection, modelview);
		calibration_cube.render(projection, modelview);

		if (leftMolecule != nullptr) {
			leftMolecule->isCo2 = false;
			leftMolecule->m = &mm.o2;
			mm.count--;
		}

		if (rightMolecule != nullptr) {
			rightMolecule->isCo2 = false;
			rightMolecule->m = &mm.o2;
			mm.count--;
		}

		for (int i = 0; i < mm.molecules.size(); i++) {
			if (!mm.molecules[i]->isCo2) {
				mm.molecules.erase(mm.molecules.begin() + i);
			}
		}

		if (buttonB) {
			//bm.createBullet();
		}
	}
};

#endif