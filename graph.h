#include "libraries.h"

class ClusterNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Cluster not found"; 
        } 
};

bool operator<(const Cluster &c1, const Cluster &c2) {
	return c1.time < c2.time;
}

bool operator<(const Vehicle &v1, const Vehicle &v2) {
	return v1.t < v2.t;
}

bool priorityById(const Vehicle& v1, const Vehicle& v2) {
	return v1.id < v2.id;
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

        void adicionarVertice(tipo id, string lat, string lon, int dem, int etw, int ltw, int dur, tipo p, tipo d, int cluster) {
            Node vertice(id, lat, lon, dem, etw, ltw, dur, p, d, cluster);
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
			// <vehicle>;<origin>,<destiny1>,<destiny2>,...,<destinyN>,<origin>
			ifstream file(solutionFileName+".txt");
			string id;

			while(getline(file, id, ';')) {
				int vehicleId = stoi(id);
				cout << endl << "Veículo [" << vehicleId << "]" << endl << endl;
				Vehicle veiculo(vehicleId);
				M.push_back(veiculo);

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
					cout << "(" << sequence[i] << ")" << endl;
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
					cout << "Janelas de tempo: ";
					// 3. Janelas de tempo
					if (M[vehicleId].t > cliente->timeWindow.second)
						return false;
					cout << "✓" << endl;
					if (M[vehicleId].t < cliente->timeWindow.first)
						M[vehicleId].t += (cliente->timeWindow.first - M[vehicleId].t);
					int duracao = cliente->dur;
					int custo = 0;
					if (i < (sequence.size()-1)) {
						Edge aresta = searchById(sequence[i], sequence[i+1]);
						custo = aresta.c;
					}

					cout << "Horário de serviço: ";
					// 2. Horário de serviço
					if ((M[vehicleId].t += duracao + custo) > ROUTE_TIME)
						return false;
					cout << "✓" << endl;
					cout << "Capacidade do veículo: ";
					// 6. Capacidade do veículo
					if ((M[vehicleId].q += cliente->dem) > CAPACITY)
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
			return M[0];
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
			return A[0]; // Retorna aresta loop no depósito (padrão)
		}

		Node& searchById(tipo u) {
			for (int i = 0; i < V.size(); i++)
				if (V[i].id == u)
					return V[i];
			return V[0]; // Retorna o depósito (padrão)
		}

		void groupByCluster() {
            for (int i = 0; i < V.size(); i++) {
                try {
                    searchClusterById(V[i].cluster).adicionaA(V[i]);
                } catch(ClusterNotFound cnf) {
					if (V[i].cluster != -1) {
						CLUSTERS++;
						Cluster cluster(V[i].cluster);
						cluster.adicionaA(V[i]);
						clusters.push_back(cluster);
					}
                }
            }
        }

		// RESOLVE START

		string resolve() {

			groupByCluster();

			for (int c = 0; c < clusters.size(); c++)
				clusters[c].time = routingTime(searchById(0), clusters[c]);

			sort(clusters.begin(), clusters.end());

			int m = CLUSTERS*0.23;
			string solutionFileName = NAME+"(solution)";

			//do {
				ofstream solution(solutionFileName+".txt");
				//m++;
				for (int v = 0; v < m; v++) {
					M.push_back(v);
					M[v].t = 0;
					M[v].q = 0;
					M[v].trajectory.clear();
					M[v].trajectory.push_back(searchById(0));
					M[v].position = searchById(0);
				}

				for (int c = 0; c < clusters.size(); c++) {
					clusters[c].available = true;
				}

				while (verifyAvailable()) {
					Cluster *a = &nearestClusterAvailable(M.front().position);
					concatenateTrajectory(M.front().trajectory, routing(M.front().position, *a));
					M.front().position = M.front().trajectory.back();
					M.front().t = routingTime(M.front().trajectory);
					a->available = false;
					sort(M.begin(), M.end());
				}
				sort(M.begin(), M.end(), priorityById);
				for (int v = 0; v < M.size(); v++) {
					M[v].trajectory.push_back(searchById(0));
					solution << M[v].id << ";";
					for (int t = 0; t < M[v].trajectory.size(); t++) {
						if(t == M[v].trajectory.size()-1)
							solution << M[v].trajectory[t].id;
						else
							solution << M[v].trajectory[t].id << ",";
					}
					solution << endl;
					M[v].t += routingTime(M[v].position, searchById(0));
				}

				M.clear();
			//} while(!verifySolution(solutionFileName));
			return solutionFileName;
		}
		

		int routingTime(Node &origem, Node &destino) { // DONE
			return searchById(origem.id, destino.id).c;
		}

		int routingTime(vector<Node>& trajectory) { // DONE
			int time = 0;
			for (int i = 0; i < trajectory.size()-1; i++)
				time += trajectory[i].dur + searchById(trajectory[i].id, trajectory[i+1].id).c;
			return time;
		}

		vector<Node> routing(Node &origem, Cluster &cluster) { // ALMOST ALL DONE
			vector<Node> collectionRoute;
			vector<Node> deliveryRoute;
			for (int i = 0; i < cluster.vertices.size(); i++) {
				collectionRoute.push_back(searchById(cluster.vertices[i]->p));
				deliveryRoute.push_back(searchById(cluster.vertices[i]->id));
			}
			//mergeSort(collectionRoute, 0, collectionRoute.size(), origem);
			concatenateTrajectory(collectionRoute, deliveryRoute);
			return collectionRoute;
		}

		int routingTime(Node &origem, Cluster &cluster) { // ALMOST ALL DONE
			vector<Node> trajectory = routing(origem, cluster);
			return routingTime(trajectory);
		}

		Cluster& nearestClusterAvailable(Node &origem) { // DONE
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

		void concatenateTrajectory(vector<Node>& tr1, vector<Node> tr2) { // DONE
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

		// MERGE SORT

		bool lessOrEqual(Node& L, Node& R, Node& origem) { // TO CHECK
    		int quotientL = (routingTime(L, searchById(L.d)))/(routingTime(origem, L));
			int quotientR = (routingTime(R, searchById(R.d)))/(routingTime(origem, R));
			return quotientR <= quotientL;
		}

		void merge(vector<Node>& arr, int l, int m, int r, Node& origem) { // TO CHECK
			int i, j, k;
			int n1 = m - l + 1;
			int n2 = r - m;

			vector<Node> L(n1), R(n2);

			for (i = 0; i < n1; i++) {
				L[i] = arr[l + i];
			}
			for (j = 0; j < n2; j++) {
				R[j] = arr[m + 1 + j];
			}

			i = 0;
			j = 0;
			k = l;
			while (i < n1 && j < n2) {
				if (lessOrEqual(L[i], R[j], origem)) {
					arr[k] = L[i];
					i++;
				} else {
					arr[k] = R[j];
					j++;
				}
				k++;
			}

			while (i < n1) {
				arr[k] = L[i];
				i++;
				k++;
			}

			while (j < n2) {
				arr[k] = R[j];
				j++;
				k++;
			}
		}

		void mergeSort(vector<Node>& arr, int l, int r, Node& origem) {
			if (l < r) {
				int m = l + (r - l) / 2;
				mergeSort(arr, l, m, origem);
				mergeSort(arr, m + 1, r, origem);
				merge(arr, l, m, r, origem);
			}
		}

		// END MERGE SORT

        void imprimir() {
			for (int i = 0; i < (int)A.size(); i++) {
				cout << "[" << A[i].origem->id << "(" << A[i].origem->dur << ")" << "," << A[i].destino->id << "(" << A[i].destino->dur << ")" << "]->c(" << A[i].c << ")" << endl;
			}
		}

		void imprimirClusters() {
			for (int i = 0; i < clusters.size(); i++) {
				for (int j = 0; j < clusters[i].vertices.size(); j++) {
					cout << clusters[i].vertices[j]->id << "->" << clusters[i].vertices[j]->cluster << endl;
				}
			}
		}
};