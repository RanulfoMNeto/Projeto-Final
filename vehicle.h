#include "libraries.h"

class Vehicle {
	public:
		tipo id;
		int q, t, posicao; // Capacidade(q) / Tempo(t)
		Vehicle(tipo id) {
			this->id = id;
			this->q = 0;
			this->t = 0;
			this->posicao = 0;
		}
};