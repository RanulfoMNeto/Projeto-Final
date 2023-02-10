#include "libraries.h"

class Vehicle {
	public:
		tipo id;
		int q, t; // Capacidade(q) / Tempo(t)
		vector<Node*> trajectory;
		Node *position;
		Vehicle(tipo id) {
			this->id = id;
			this->q = 0;
			this->t = 0;
		}
};