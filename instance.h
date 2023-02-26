#include "libraries.h"

class Instance {
    public:
        string name; // Nome da instância
        int M; // Número de veículos gastos pela melhor solução conhecida
        int C; // Custo gasto pelos veículos da melhor solução conhecida
        int clusters;
        Graph *graph;
        pair<double, double> deviation;
        Instance(string name, int M, int C, int clusters) {
            this->name = name;
            this->M = M;
            this->C = C;
            this->clusters = clusters;
        }

        void calculateDeviation() {
            cout << graph->M.size() << "-" << M << endl;
            deviation.first = ((graph->M.size() - M)/(double)M) * (double)100;
            deviation.second = ((graph->totalRoutingTime() - C)/(double)C) * (double)100;
        }
};