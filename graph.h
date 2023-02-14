#include "libraries.h"

class EdgeNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Edge not found"; 
        } 
};

class VehicleNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Vehicle not found"; 
        } 
};

bool operator<(const Vehicle &v1, const Vehicle &v2) {
	return v1.t < v2.t;
}

bool vehiclePriorityById(const Vehicle& v1, const Vehicle& v2) {
	return v1.id < v2.id;
}

class NodeNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Node not found"; 
        } 
};

bool nodePriorityByDistance(const Node *n1, const Node *n2) {
	return n1->distance < n2->distance;
}

bool nodePriorityByTimeWindow(const Node *n1, const Node *n2) {
	return n1->timeWindow.first < n2->timeWindow.first;
}

bool nodePriorityByDemand(const Node *n1, const Node *n2) {
	return n1->dem < n2->dem;
}

class ClusterNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Cluster not found"; 
        } 
};

bool clusterPriorityByDemand(const Cluster &c1, const Cluster &c2) {
	return c1.dem < c2.dem;
}

bool clusterPriorityById(const Cluster &c1, const Cluster &c2) {
	return c1.id < c2.id;
}

bool clusterPriorityByTimeWindow(const Cluster &c1, const Cluster &c2) {
	return (c1.timeWindow.first) < (c2.timeWindow.first);
}


class Graph {
	public:
		int SIZE, CAPACITY, ROUTE_TIME, CLUSTERS;
		string NAME;
        vector<Node> V; // Conjunto de vértices (Coleta U Entrega U Depósito)
		vector<Edge> A;
		vector<Vehicle> M;
		vector<Cluster> clusters;

		Graph() {
			CLUSTERS = 0;
		}

        void adicionarVertice(tipo id, string lat, string lon, int dem, int etw, int ltw, int dur, tipo p, tipo d) {
            Node vertice(id, lat, lon, dem, etw, ltw, dur, p, d);
            V.push_back(vertice);
        }

		bool adicionarAresta(tipo uId, tipo vId, int custo) {
            Node *u;
            Node *v;
			bool uE = false;
			bool vE = false;
            for (int i = 0; i < (int)V.size(); i++) {
				if (V[i].id == uId) {
					u = &V[i];
					uE = true;
				}
				if (V[i].id == vId) {
					v = &V[i];
					vE = true;
				}
				if (uE && vE) {
					Edge aresta(*u, *v, custo);
            		A.push_back(aresta);
					return true;
				}
            }
			return false;
            
		}

		bool verifySolution(string solutionFileName) {

			M.clear();

			for (int i = 0; i < V.size(); i++)
				V[i].vehicleId = -1;

			// <vehicle>;<origin>,<destiny1>,<destiny2>,...,<destinyN>,<origin>
			ifstream file(solutionFileName+".txt");
			string id;

			while(getline(file, id, ';')) {
				int vehicleId = stoi(id);

				try {
					searchVehicleById(vehicleId);
				} catch(VehicleNotFound vnf) {
					M.push_back(vehicleId);
				}

				cout << endl << "Veículo [" << vehicleId << "]" << endl << endl;

				string route;
				getline(file, route);
				stringstream sst(route);

				string clientId;
				vector<int> sequence;

				while(getline(sst, clientId, ',')) {
					sequence.push_back(stoi(clientId));
				}

				cout << "Origem de serviço: ";
				// 2. Origem de serviço
				if (sequence[0] or sequence[sequence.size()-1])
					return false;
				cout << "✓" << endl;

				for (int i = 0; i < sequence.size(); i++) {
					cout << "{" << sequence[i] << "}" << endl;
					Node *cliente = &searchById(sequence[i]);
					if (cliente->id != 0) { // Vértice não é o depósito
						cout << "Exclusividade de visita: ";
						// 4. Exclusividade de visita
						if (cliente->vehicleId != -1)
							return false;
						cout << "✓" << endl;
						cliente->vehicleId = vehicleId;

						cout << "Atendimento de pedido e Precedência de coleta e entrega: ";
						// 5. Atendimento de pedido
						// 1. Precedência de coleta e entrega
						if (cliente->dem < 0) {
							Node coleta = searchById(cliente->p);
							if (cliente->vehicleId != coleta.vehicleId)
								return false;
						}
						cout << "✓" << endl;
					}
					cout << "(" << searchVehicleById(vehicleId).t << ")Janelas de tempo: " << cliente->id;
					cout << endl << "[" << cliente->timeWindow.first << "," << cliente->timeWindow.second << "]:";
					// 3. Janelas de tempo
					if (searchVehicleById(vehicleId).t > cliente->timeWindow.second)
						return false;
					cout << "✓" << endl;
					if (searchVehicleById(vehicleId).t < cliente->timeWindow.first)
						searchVehicleById(vehicleId).t += (cliente->timeWindow.first - searchVehicleById(vehicleId).t);
					int duracao = cliente->dur;
					int custo = 0;
					if (i < (sequence.size()-1)) {
						Edge aresta = searchById(sequence[i], sequence[i+1]);
						custo = aresta.c;
					}

					cout << "Horário de serviço: ";
					// 2. Horário de serviço
					if ((searchVehicleById(vehicleId).t += duracao + custo) > ROUTE_TIME)
						return false;
					cout << "✓" << endl;
					cout << "Capacidade do veículo: ";
					// 6. Capacidade do veículo
					if ((searchVehicleById(vehicleId).q += cliente->dem) > CAPACITY)
						return false;
					cout << "✓" << endl;
				}

			}

			cout << "Obrigatoriedade de visita: ";
			// 4. Obrigatoriedade de visita
			for (int i = 0; i < V.size(); i++) {
				if (V[i].id != 0) {
					if (V[i].vehicleId == -1)
						return false;
				}
			}
			cout << "✓" << endl;

			return true;

		}

		Vehicle& searchVehicleById(int id) {
			for (int i = 0; i < M.size(); i++) {
                if (M[i].id == id)
                    return M[i];
			}
			throw VehicleNotFound();
		}

        Cluster& searchClusterById(int id) {
            for (int i = 0; i < clusters.size(); i++) {
                if (clusters[i].id == id)
                    return clusters[i];
            }
            throw ClusterNotFound();
        }

		Edge& searchById(tipo u, tipo v) {
			for (int i = 0; i < A.size(); i++)
				if ((A[i].origem->id == u) and (A[i].destino->id == v))
					return A[i];
			throw EdgeNotFound();
		}

		Node& searchById(tipo u) {
			for (int i = 0; i < V.size(); i++)
				if (V[i].id == u)
					return V[i];

			//return V[u];
			throw NodeNotFound();
		}
		
		// CLUSTERING START

		void clear() {
			CLUSTERS = 0;
			M.clear();
			clusters.clear();
			for (int i = 0; i < V.size(); i++)
				V[i].cluster = -1;
		}

		void groupByCluster() {
			clusters.clear();
			CLUSTERS = 0;

            for (int i = 0; i < V.size(); i++) {
                try {
                    searchClusterById(V[i].cluster).adicionaA(V[i]);
                } catch(ClusterNotFound cnf) {
					if (V[i].cluster != -1) {
						CLUSTERS++;
						Cluster cluster(V[i].cluster);
						clusters.push_back(cluster);
						searchClusterById(V[i].cluster).adicionaA(V[i]);
					}
                }
            }
			for (int c = 0; c < clusters.size(); c++) {
				for (int i = 0; i < clusters[c].vertices.size(); i++) {
					clusters[c].timeWindow.first = (clusters[c].vertices[i]->timeWindow.first < clusters[c].timeWindow.first)?clusters[c].vertices[i]->timeWindow.first:clusters[c].timeWindow.first;

					clusters[c].timeWindow.second = (clusters[c].vertices[i]->timeWindow.second > clusters[c].timeWindow.second)?clusters[c].vertices[i]->timeWindow.second:clusters[c].timeWindow.second;
				}
			}
        }

		void clustering() {
			int number = 0;
			vector<Node*> deliveryNodes = selectDeliveryNodes();
			do {
				number++;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				cout << "============== " << number << " ==============" << endl;
				for (int i = 0; i < V.size(); i++)
					V[i].cluster= -1;
				kMeansClustering(deliveryNodes, number);
				clusters.clear();
				groupByCluster();
				sort(clusters.begin(), clusters.end(), clusterPriorityById);
				for (int c = 0; c < clusters.size(); c++) {
					int sum = 0;
					for (int d = 0; d < clusters[c].vertices.size(); d++) {
						sum += abs(clusters[c].vertices[d]->dem);
					}
					cout << clusters[c].id << "=>" << sum << endl;
				}
			} while(demandGreaterThan3Capacity());
			int l = 0;
			do {
				cout << "=============== regroup =================" << endl;
				cout << "================ " << l++ << " ================" << endl;
				cout << "================================" << endl;
				regroup();
			} while(demandGreaterThanCapacity());
		}

		void regroup() {
			for (int c = 0; c < clusters.size(); c++) {
				calculateDemand();
				if (clusters[c].dem > CAPACITY) {
					Cluster *cluster = &clusters[c];
					sort(clusters[c].vertices.begin(), clusters[c].vertices.end(), nodePriorityByDemand);
					Node *higher = cluster->vertices.back();
					bool inserted = false;
					sort(clusters.rbegin(), clusters.rend(), clusterPriorityByDemand);
					for (int n = 0; (n < clusters.size()) && (!inserted); n++) {
						if ((clusters[n].dem+abs(higher->dem)) < CAPACITY) {
							higher->cluster = clusters[n].id;
							inserted = true;
						}
					}
					if (!inserted) {
						higher->cluster = CLUSTERS++;
					}
					groupByCluster();
				}
			}
		}

		bool demandGreaterThan3Capacity() {
			for (int c = 0; c < clusters.size(); c++) {
				int sum = 0;
				for (int d = 0; d < clusters[c].vertices.size(); d++) {
					sum += abs(clusters[c].vertices[d]->dem);
				}
				if (sum > 3*CAPACITY)
					return true;
			}
			return false;
		}

		bool demandGreaterThanCapacity() {
			for (int c = 0; c < clusters.size(); c++) {
				int sum = 0;
				for (int d = 0; d < clusters[c].vertices.size(); d++) {
					sum += abs(clusters[c].vertices[d]->dem);
				}
				if (sum > CAPACITY)
					return true;
			}
			return false;
		}

		void calculateDemand() {
			for (int c = 0; c < clusters.size(); c++) {
				clusters[c].dem = 0;
				for (int d = 0; d < clusters[c].vertices.size(); d++) {
					clusters[c].dem += abs(clusters[c].vertices[d]->dem);
				}
			}
		}

		// CLUSTERING END

		// RESOLVE START

		bool resolve() {

			sort(clusters.begin(), clusters.end(), clusterPriorityByTimeWindow);

			string folder = "solutions/";
			string solutionFileName = folder+NAME+"(solution)";
			int m = 1;
			bool solved = true;
			cout << "CLUSTERS: " << CLUSTERS << endl;
			do {
				M.clear();
				ofstream solution(solutionFileName+".txt");
				m++;

				for (int v = 0; v < m; v++) {
					M.push_back(v);
					M[v].trajectory.clear();
					M[v].trajectory.push_back(&searchById(0));
					M[v].position = &searchById(0);
					M[v].t = 0;
					//M[v].q = 0;
				}

				for (int c = 0; c < clusters.size(); c++) {
					clusters[c].available = true;
				}
				while (verifyAvailable()) {
					sort(M.begin(), M.end()); // priority by time
					Cluster *a = &nearestClusterAvailable(*M.front().position);
					a->available = false;
					concatenateTrajectory(M.front().trajectory, routing(*M.front().position, *a));
					M.front().position = M.front().trajectory.back();
					M.front().t = routingTime(M.front().trajectory);
				}

				sort(M.begin(), M.end(), vehiclePriorityById);
				for (int v = 0; v < M.size(); v++) {
					M[v].trajectory.push_back(&searchById(0));
					M[v].t = routingTime(M[v].trajectory);
					solution << M[v].id << ";";
					cout << "V[" << M[v].id << "] = " << M[v].t  << endl;
					for (int t = 0; t < M[v].trajectory.size(); t++) {
						if(t == M[v].trajectory.size()-1)
							solution << M[v].trajectory[t]->id;
						else
							solution << M[v].trajectory[t]->id << ",";
					}
					solution << endl;
				}
				if (m == CLUSTERS) {
					solved = false;
					break;
				}
			} while(!verifySolution(solutionFileName));
			return solved;

		}

		int routingTime(Node &origem, Node &destino) { // DONE
			return searchById(origem.id, destino.id).c;
		}

		int routingTime(vector<Node*>& trajectory) { // DONE
			int time = 0;
			for (int i = 0; i < trajectory.size()-1; i++)
				time += searchById(trajectory[i]->id, trajectory[i+1]->id).c + trajectory[i+1]->dur;
			return time;
		}

		vector<Node*> routing(Node &origem, Cluster &cluster) { // DONE
			vector<Node*> collectionRoute;
			vector<Node*> deliveryRoute;
			for (int i = 0; i < cluster.vertices.size(); i++) {
				deliveryRoute.push_back(cluster.vertices[i]);
				collectionRoute.push_back(&searchById(cluster.vertices[i]->p));
			}
			for (int i = 0; i < collectionRoute.size(); i++)
				collectionRoute[i]->distance = (searchById(collectionRoute[i]->id, collectionRoute[i]->d).c)/(searchById(origem.id, collectionRoute[i]->d).c);
			
			//sort(collectionRoute.begin(), collectionRoute.end(), timeWindowPriority);
			//sort(deliveryRoute.begin(), deliveryRoute.end(), timeWindowPriority);
			
			concatenateTrajectory(collectionRoute, deliveryRoute);

			sort(collectionRoute.begin(), collectionRoute.end(), nodePriorityByTimeWindow);

			return collectionRoute;
		}

		int routingTime(Node &origem, Cluster &cluster) { // DONE
			vector<Node*> trajectory = routing(origem, cluster);
			return routingTime(trajectory);
		}

		Cluster& nearestClusterAvailable(Node &origem) { // ALMOST ALL DONE
			int c;
			pair<int,int> nearest; // <cluster, shorter>
			nearest.second = numeric_limits<int>::max();
			for (c = 0; c < clusters.size(); c++) {
                if (clusters[c].available) {
					int shorter = numeric_limits<int>::max();
					for (int j = 0; j < clusters[c].vertices.size(); j++) {
						int time = routingTime(searchById(origem.id), searchById(clusters[c].vertices[j]->p));
						shorter = (time<shorter)?time:shorter;
            		}
					if (shorter < nearest.second) {
						nearest.first = clusters[c].id;
						nearest.second = shorter;
					}
				}
            }
            return searchClusterById(nearest.first);
		}

		void concatenateTrajectory(vector<Node*>& tr1, vector<Node*> tr2) { // DONE
			for (int i = 0; i < tr2.size(); i++)
				tr1.push_back(tr2[i]);
		}
		
		bool verifyAvailable() { // DONE
			for (int j = 0; j < clusters.size(); j++) {
				if(clusters[j].available) {
					return true;
				}
			}
			return false;
		}

		// RESOLVE END

		vector<Node*> selectDeliveryNodes() {
			vector<Node*> deliveryNodes;
			for (int i = 0; i < V.size(); i++) {
				if (V[i].dem < 0) {
					deliveryNodes.push_back(&V[i]);
				}
			}
			return deliveryNodes;
		}

        void imprimir() {
			for (int i = 0; i < (int)A.size(); i++) {
				cout << "[" << A[i].origem->id << "(" << A[i].origem->dur << ")" << "," << A[i].destino->id << "(" << A[i].destino->dur << ")" << "]->c(" << A[i].c << ")" << endl;
			}
		}
		void imprimirClustersById() {
			sort(clusters.begin(), clusters.end(), clusterPriorityById);
			for (int i = 0; i < clusters.size(); i++) {
				cout << "[" << clusters[i].id << "](" << clusters[i].dem << ")" << ":";
				for (int j = 0; j < clusters[i].vertices.size(); j++) {
					cout << clusters[i].vertices[j]->id << " ";
				}
				cout << endl;
			}
		}
		void imprimirClustersByTimeWindow() {
			for (int c = 0; c < clusters.size(); c++) {
				for (int j = 0; j < clusters[c].vertices.size(); j++)
					clusters[c].timeWindow.first = (clusters[c].vertices[j]->timeWindow.first < clusters[c].timeWindow.first)?clusters[c].vertices[j]->timeWindow.first:clusters[c].timeWindow.first;
			}
			sort(clusters.begin(), clusters.end(), clusterPriorityByTimeWindow);
			for (int i = 0; i < clusters.size(); i++) {
				cout << "[" << clusters[i].id << "](" << clusters[i].timeWindow.first << "):";
				for (int j = 0; j < clusters[i].vertices.size(); j++) {
					cout << clusters[i].vertices[j]->id << " ";
				}
				cout << endl;
			}
		}

		void write() {
			ofstream output("outputs/"+NAME+"(output).csv");
			output << "id,latitude,longitude,cluster,vehicle" << endl;
			for (int i = 0; i < V.size(); i++)
				output << V[i].id << "," << V[i].lat << "," << V[i].lon << "," << V[i].cluster << "," << V[i].vehicleId << endl;
		}
};