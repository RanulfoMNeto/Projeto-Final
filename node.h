#include "libraries.h"

class Node {
    public:
		tipo id, p, d, vehicleId;
		string lat, lon;
		ii timeWindow;
		int dem, dur, cluster;
		Node();
        Node(tipo id, string lat, string lon, int dem, int etw, int ltw, int dur, tipo p, tipo d, int cluster) {
            this->id = id;
			this->lat = lat;
			this->lon = lon;
			this->dem = dem;
			timeWindow = make_pair(etw, ltw);
			this->dur = dur;
			this->p = p;
			this->d = d;
			this->cluster = cluster;
			vehicleId = -1;
        }
		bool operator==(Node interseccao) {  
			return this->id == interseccao.id;
		}
};