#include "libraries.h"

class Instance {
    public:
        string I;
        int M, C;
        Graph graph;
        Instance(string I, int M, int C) {
            this->I = I;
            this->M = M;
            this->C = C;
        }
};