#include "libraries.h"

class Edge {
    public:
        Node *origem;
		Node *destino;
        int c;
		Edge() {}
        Edge(Node &origem, Node &destino, int tempo) {
            this->origem = &origem;
            this->destino = &destino;
            this->c = tempo; // Custo é diretamente proporcional ao tempo de deslocamento
        }
};