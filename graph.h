#include "libraries.h"

class ImpossibleTrajectory : public exception {
	private:
    	char * message;
    public:
		ImpossibleTrajectory(char * msg) : message(msg) {}
        const char* what() const throw() {
			return message;
        } 
};

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

bool vehiclePriorityByTime(const Vehicle &v1, const Vehicle &v2) {
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

bool clusterPriorityByDegree(const Cluster &c1, const Cluster &c2) {
	return (c1.vertices.size()) < (c2.vertices.size());
}


class Graph {
	public:
		int SIZE, CAPACITY, ROUTE_TIME, CLUSTERS;
		string NAME;
        vector<Node> V; // Conjunto de vértices (Coleta U Entrega U Depósito)
		vector<Edge> A;
		vector<Vehicle> M;
		vector<Cluster> clusters;
		vector<vector<int> > AM;

		Graph() {
			CLUSTERS = 0;
		}

        void addNode(tipo id, string lat, string lon, int dem, int etw, int ltw, int dur, tipo p, tipo d) {
            Node vertice(id, lat, lon, dem, etw, ltw, dur, p, d);
            V.push_back(vertice);
        }

		void addEdge(tipo u, tipo v, int custo) {
			Edge aresta(searchById(u), searchById(v), custo);
			A.push_back(aresta);
		}

		bool verifySolution(string solutionFileName) {

			//M.clear();
			// CHANGED
			for (int i = 0; i < M.size(); i++) {
				M[i].t = 0;
			}

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
						//Edge aresta = searchById(sequence[i], sequence[i+1]);
						//custo = aresta.c;
						custo = AM[sequence[i]][sequence[i+1]];
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
			/*
			for (int i = 0; i < V.size(); i++)
				if (V[i].id == u)
					return V[i];
			*/
			return V[u];
			throw NodeNotFound();
		}
		
		// CLUSTERING START

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

        }

		void clustering(int number) {
			vector<Node*> deliveryNodes = selectDeliveryNodes();
			kMeansClustering(deliveryNodes, number);
			//insertCluster();
			groupByCluster();
		}

		// CLUSTERING END

		bool verifyTrajectory(vector<Node*> trajectory) {

			Vehicle vehicle(0); // tester vehicle

			for (int i = 0; i < trajectory.size(); i++)
				trajectory[i]->vehicleId = -1;

			for (int i = 0; i < trajectory.size(); i++) {
				//cout << "{" << trajectory[i]->id << "}" << endl;
				if (trajectory[i]->id != 0) { // Vértice não é o depósito
					if (trajectory[i]->vehicleId != -1)
						throw ImpossibleTrajectory((char *)"verifyTrajectory: 4. Exclusividade de visita");
					trajectory[i]->vehicleId = vehicle.id;

					if (trajectory[i]->dem < 0) {
						if (trajectory[i]->vehicleId != searchById(trajectory[i]->p).vehicleId)
							throw ImpossibleTrajectory((char *)"verifyTrajectory: 5. Atendimento de pedido; 1. Precedência de coleta e entrega");
					}
				}

				if (vehicle.t > trajectory[i]->timeWindow.second)
					throw ImpossibleTrajectory((char *) "verifyTrajectory: 3. Janelas de tempo");
				if (vehicle.t < trajectory[i]->timeWindow.first)
					vehicle.t += (trajectory[i]->timeWindow.first - vehicle.t);
				int duracao = trajectory[i]->dur;
				int custo = 0;
				if (i < (trajectory.size()-1)) {
					custo = AM[trajectory[i]->id][trajectory[i+1]->id];
				}
				
				if ((vehicle.t += duracao + custo) > ROUTE_TIME)
					throw ImpossibleTrajectory((char *)"verifyTrajectory: 2. Horário de serviço");

				if ((vehicle.q += trajectory[i]->dem) > CAPACITY)
					throw ImpossibleTrajectory((char *)"verifyTrajectory: 6. Capacidade do veículo");

			}
			//cout << endl;
			return true;
		}


		// SOLVE START

		bool solve() {

			M.clear();

			for (int c = 0; c < clusters.size(); c++) {
				clusters[c].available = true;
				clusters[c].visited = false;
				for (int v = 0; v < clusters[c].vertices.size(); v++) {
					clusters[c].vertices[v]->visited = false;
					clusters[c].vertices[v]->inserted = false;
				}
			}

			int m = 0;

			while (verifyAvailableCluster()) {

				M.push_back(m++);
				M.back().trajectory.clear();
				M.back().trajectory.push_back(&searchById(0));
				M.back().position = &searchById(0);
				M.back().t = 0;
				//sort(M.begin(), M.end(), vehiclePriorityByTime);
				Vehicle *vehicle = &M.back();

				while (verifyUnvisitedCluster()) {

					Cluster *cluster = &nearestUnvisitedAvailableCluster(*vehicle->position);
					cluster->visited = true;
					vector<Node*> trajectory = vehicle->trajectory;

					sort(cluster->vertices.begin(), cluster->vertices.end(), nodePriorityByTimeWindow);
					for (int v = 0; v < cluster->vertices.size(); v++) {
						if (cluster->vertices[v]->visited == false) {
							cluster->vertices[v]->visited = true;
							trajectory.push_back(&searchById(0));
							nearestInsertion(trajectory, *cluster->vertices[v]);
							try {
								//trajectory.push_back(&searchById(0));//
								verifyTrajectory(trajectory);
								trajectory.pop_back();
								vehicle->trajectory = trajectory;
								cluster->vertices[v]->inserted = true;
								vehicle->position = vehicle->trajectory.back();
							} catch (ImpossibleTrajectory it) {
								cluster->vertices[v]->inserted = false;
								trajectory = vehicle->trajectory;
							}

						}
					}
					
					cluster->available = availability(*cluster);

				}
				vehicle->trajectory.push_back(&searchById(0));
				for (int c = 0; c < clusters.size(); c++) {
					clusters[c].available = false;
					for (int v = 0; v < clusters[c].vertices.size(); v++) {
						if (clusters[c].vertices[v]->inserted == false) {
							clusters[c].available = true;
							clusters[c].visited = false;
							clusters[c].vertices[v]->visited = false;
						}
					}
				}
			}
			string solutionFileName = writeSolution("(solution)");
			return verifySolution(solutionFileName);
		}

		// ====================
		
		// FUNCTIONS FOR SOLVE

		void nearestInsertion(vector<Node*> &trajectory, Node &delivery) {

			/*
				min {Dij + Djk - Dik}
				(i precede k imediatamente)
			*/

			Node *pickup = &searchById(delivery.p);
			pair<int, int> minPickup = make_pair(numeric_limits<int>::max(), 0);
			for (int i = 0; i < trajectory.size(); i++) {
				if (i < trajectory.size()-1) {
					int Dij = AM[trajectory[i]->id][pickup->id];
					int Djk = AM[pickup->id][trajectory[i+1]->id];
					int Dik = AM[trajectory[i]->id][trajectory[i+1]->id];
					if ((Dij + Djk - Dik) < minPickup.first) {
						minPickup.first = Dij + Djk - Dik;
						minPickup.second = i+1;
					}
				}
			}
			trajectory.insert(trajectory.begin()+minPickup.second, pickup);

			pair<int, int> minDelivery = make_pair(numeric_limits<int>::max(), 0);
			for (int i = minPickup.second; i < trajectory.size(); i++) {
				if (i < trajectory.size()-1) {
					int Dij = AM[trajectory[i]->id][delivery.id];
					int Djk = AM[delivery.id][trajectory[i+1]->id];
					int Dik = AM[trajectory[i]->id][trajectory[i+1]->id];
					if ((Dij + Djk - Dik) < minDelivery.first) {
						minDelivery.first = Dij + Djk - Dik;
						minDelivery.second = i+1;
					}
				}
			}
			trajectory.insert(trajectory.begin()+minDelivery.second, &delivery);
		}

		int routingTime(Node &origem, Node &destino) {
			//return searchById(origem.id, destino.id).c;
			return AM[origem.id][destino.id];
		}

		int routingTime(vector<Node*>& trajectory) {
			Vehicle vehicle(0);
			for (int i = 0; i < trajectory.size(); i++) {
				if (vehicle.t < trajectory[i]->timeWindow.first)
					vehicle.t += (trajectory[i]->timeWindow.first - vehicle.t);
				int duracao = trajectory[i]->dur;
				int custo = 0;
				if (i < (trajectory.size()-1)) {
					//Edge aresta = searchById(trajectory[i]->id, trajectory[i+1]->id);
					//custo = aresta.c;
					custo = AM[trajectory[i]->id][trajectory[i+1]->id];
				}
				vehicle.t += duracao + custo;
			}

			return vehicle.t;

		}

		Node* nearestNodeUnvisited(vector<Node*> nodes, Vehicle &vehicle) { // UNUSED FUNCTION
			Node *origem = vehicle.position;
			Node* nearestNode;
			double shorter = numeric_limits<double>::max();
			for (int i = 0; i < nodes.size(); i++) {
				if (!nodes[i]->visited) {
					int elapsedTime = vehicle.t;
					int timeWindowFirst = nodes[i]->timeWindow.first;
					int time = searchById(origem->id, nodes[i]->id).c;
					//int value = timeWindowFirst-(time+elapsedTime);
					int value = timeWindowFirst;
					//cout << nodes[i]->id << " : " << timeWindowFirst << "-(" << time << "+" << elapsedTime << ")=" << value << endl;
					if (value < shorter) {
						shorter = value;
						nearestNode = nodes[i];
					}
				}
			}
			vehicle.t += (searchById(origem->id, nearestNode->id).c + nearestNode->dur);
			//cout << endl << "Menor Destino: " << nearestNode->id << "[" << shorter << "]" << endl << endl;
			return nearestNode;
		}

		Cluster& nearestUnvisitedAvailableCluster(Node &origem) {
			int c;
			pair<int,int> nearest; // <cluster, shorter>
			nearest.second = numeric_limits<int>::max();
			for (c = 0; c < clusters.size(); c++) {
                if ((clusters[c].available == true) and (clusters[c].visited == false)) {
					int shorter = numeric_limits<int>::max();
					for (int j = 0; j < clusters[c].vertices.size(); j++) {
						int time = routingTime(searchById(origem.id), searchById(clusters[c].vertices[j]->p));
						shorter = (time < shorter)?time:shorter;
            		}
					if (shorter < nearest.second) {
						nearest.first = clusters[c].id;
						nearest.second = shorter;
					}
				}
            }
            return searchClusterById(nearest.first);
		}
		
		bool verifyAvailableCluster() { // DONE
			for (int i = 0; i < clusters.size(); i++) {
				if (clusters[i].available == true) {
					return true;
				}
			}
			return false;
		}

		bool verifyUnvisitedCluster() { // DONE
			for (int i = 0; i < clusters.size(); i++) {
				if (clusters[i].visited == false) {
					return true;
				}
			}
			return false;
		}

		bool availability(Cluster &cluster) {
			for (int v = 0; v < cluster.vertices.size(); v++) {
				if (cluster.vertices[v]->inserted == false) {
					return true;
				}
			}
			return false;
		}

		string writeSolution(string ext) {
			string fileName = "solutions/"+NAME+ext;
			ofstream solution(fileName+".txt");
			for (int m = 0; m < M.size(); m++) {
				solution << M[m].id << ";";
				for (int t = 0; t < M[m].trajectory.size(); t++) {
					solution << M[m].trajectory[t]->id << ",";
				}
				solution << endl;
			}
			return fileName;
		}

		vector<Node*> selectDeliveryNodes() {
			vector<Node*> deliveryNodes;
			for (int i = 0; i < V.size(); i++) {
				if (V[i].dem < 0) {
					deliveryNodes.push_back(&V[i]);
				}
			}
			return deliveryNodes;
		}

		// ====================

		// SOLVE END

        void print() {
			for (int i = 0; i < (int)A.size(); i++) {
				cout << "[" << A[i].origem->id << "(" << A[i].origem->dur << ")" << "," << A[i].destino->id << "(" << A[i].destino->dur << ")" << "]->c(" << A[i].c << ")" << endl;
			}
		}
		void printClustersById() {
			cout << endl << "===================== CLUSTERS =====================" << endl << endl;
			sort(clusters.begin(), clusters.end(), clusterPriorityById);
			for (int i = 0; i < clusters.size(); i++) {
				cout << "[" << clusters[i].id << "](" << clusters[i].available << "," << clusters[i].visited << ")" << ": ";
				for (int j = 0; j < clusters[i].vertices.size(); j++) {
					cout << clusters[i].vertices[j]->id << "[" << clusters[i].vertices[j]->inserted << "," << clusters[i].vertices[j]->visited << "], ";
				}
				cout << endl;
			}
		}
		void printClustersByTimeWindow() {
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
		void printFleet() {
			int total = 0;
			for (int v = 0; v < M.size(); v++) {
				cout << M[v].id << ": ";
				int vehicleRoutingTime = routingTime(M[v].trajectory);
				cout << "Tempo decorrido: " << vehicleRoutingTime << endl << endl;
				total += vehicleRoutingTime;
			}
			cout << "Total: " << total << endl << endl;
		}

		void myMaps() {
			ofstream output(NAME+"(output).csv");
			output << "id,lat,long,dem,p,cluster" << endl;
			for (int i = 0; i < V.size(); i++) {
				output << V[i].id << "," << V[i].lat << "," << V[i].lon << "," << V[i].dem << "," << V[i].p << "," << V[i].cluster << endl;
			}
		}

};