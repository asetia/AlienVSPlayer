#pragma once

#ifndef MOLECULE_MANAGER_H
#define MOLECULE_MANAGER_H

#include "Util.h"

class MoleculeManager {

public:

	Model co2;
	Model o2;
	std::vector<Molecule *> molecules;
	int count;
	bool isGameOver;

	MoleculeManager() : co2("./newdalekparadigm.obj"), o2("./o2/o2.obj") {
		count = 0;
		gameStart();
		isGameOver = false;
	}

	void gameStart() {
		count = 0;
		isGameOver = false;
		for (int i = 0; i < 5; i++) {
			molecules.push_back(new Molecule(&co2));
			count++;
		}
	}

	void gameEnd() {
		for (int i = 0; i < 100; i++) {
			molecules.push_back(new Molecule(&co2));
			for (Molecule * m : molecules) {
				m->randomizePosition();
			}
		}
	}

	void gameWon() {
		isGameOver = true;
		glClearColor(0.6f, 0.8f, 0.9f, 0.0f);
	}

	void addMolecule() {
		if (!isGameOver) {
			molecules.push_back(new Molecule(&co2));
			count++;
			/*if (count > 10) {
				isGameOver = true;
				gameEnd();
			}*/
		}
	}

	void restart() {
		molecules.clear();
		isGameOver = false;
		gameStart();
	}

	void renderMolecules(Shader shader, const mat4 & projection, const mat4 & modelview) {
		//std::cout << "drawing a dalek" << std::endl;
		for (Molecule * m : molecules) {
			m->render(shader, projection, modelview);
		}
	}
};


#endif // !MOLECULEMANAGER_H
