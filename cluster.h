#include "libraries.h"

class Cluster {

    public:
        int id, time, dem;
        bool available, visited;
        ii timeWindow;

        vector<Node*> vertices;
        void adicionaA(Node &a) {
            vertices.push_back(&a);
        }

        Cluster(int id) {
            this->id = id;
            available = true;
            visited = false;
            time = 0;
            dem = 0;
            timeWindow.first = numeric_limits<int>::max();
            timeWindow.second = 0;
        }
};