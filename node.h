#include "libraries.h"

class Node {
    public:
		tipo id, p, d, vehicleId;
		string lat, lon;
		ii timeWindow;
		int dem, dur, cluster;
		int distance;
		double minDist;  // default infinite dist to nearest cluster
		bool visited, inserted;
		Node() {
			
		}
        Node(tipo id, string lat, string lon, int dem, int etw, int ltw, int dur, tipo p, tipo d) {
            this->id = id;
			this->lat = lat;
			this->lon = lon;
			this->dem = dem;
			timeWindow = make_pair(etw, ltw);
			this->dur = dur;
			this->p = p;
			this->d = d;
			this->cluster = -1;
			vehicleId = -1;
			distance = 0;
			minDist = numeric_limits<double>::max();
			visited = false;
			inserted = false;
        }
		bool operator==(Node interseccao) {  
			return this->id == interseccao.id;
		}
		double euclideanDistance(Node p) {
            return (stof(p.lat) - stof(lat)) * (stof(p.lat) - stof(lat)) + (stof(p.lon) - stof(lon)) * (stof(p.lon) - stof(lon));
        }
};