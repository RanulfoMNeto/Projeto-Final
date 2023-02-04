#include "libraries.h"

class ClusterNotFound : public exception {
    public:
        virtual const char* what() const throw() { 
            return "Custom Exception"; 
        } 
};

bool operator<(const Cluster &c1, const Cluster &c2) {
	return c1.time < c2.time;
}

bool operator<(const Vehicle &v1, const Vehicle &v2) {
	return v1.t < v2.t;
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

				cout << "Origem de serviço" << endl;
				// 2. Origem de serviço
				if (sequence[0] or sequence[sequence.size()-1])
					return false;


				for (int i = 0; i < sequence.size(); i++) {
					cout << "(" << sequence[i] << ")" << endl;
					Node *cliente = &searchById(sequence[i]);
					if (cliente->id != 0) { // Vértice não é o depósito
						cout << "Exclusividade de visita" << endl;
						// 4. Exclusividade de visita
						if (cliente->vehicleId != -1)
							return false;
						cliente->vehicleId = vehicleId;

						cout << "Atendimento de pedido e Precedência de coleta e entrega" << endl;
						// 5. Atendimento de pedido
						// 1. Precedência de coleta e entrega
						if (cliente->dem < 0) {
							Node coleta = searchById(cliente->p);
							if (cliente->vehicleId != coleta.vehicleId)
								return false;
						}
					}
					cout << "Janelas de tempo" << endl;
					// 3. Janelas de tempo
					if (M[vehicleId].t > cliente->timeWindow.second)
						return false;
					if (M[vehicleId].t < cliente->timeWindow.first)
						M[vehicleId].t += (cliente->timeWindow.first - M[vehicleId].t);
					int duracao = cliente->dur;
					int custo = 0;
					if (i < (sequence.size()-1)) {
						Edge aresta = searchById(sequence[i], sequence[i+1]);
						custo = aresta.c;
					}

					cout << "Horário de serviço" << endl;
					// 2. Horário de serviço
					if ((M[vehicleId].t += duracao + custo) > ROUTE_TIME)
						return false;
					
					cout << "Capacidade do veículo" << endl;
					// 6. Capacidade do veículo
					if ((M[vehicleId].q += cliente->dem) > CAPACITY)
						return false;
				}

			}

			cout << "Obrigatoriedade de visita" << endl;
			// 4. Obrigatoriedade de visita
			for (int i = 0; i < V.size(); i++) {
				if (V[i].id != 0) {
					if (V[i].vehicleId == -1)
						return false;
				}
			}

			return true;

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

		void clustering() {
            for (int i = 0; i < V.size(); i++) {
                try {
                    searchClusterById(V[i].cluster).adicionaA(V[i]);
                } catch(ClusterNotFound mce) {
                    Cluster cluster(V[i].cluster);
                    cluster.adicionaA(V[i]);
                    clusters.push_back(cluster);
                }
            }
        }

		// Macroentrega 2 (Desconsiderar)
		/*
		void resolve() {

			clustering();

			for (int c = 0; c < clusters.size(); c++)
				clusters[c].time = routingTime(searchById(0), clusters[c]); // routingTime(<origem>, <todos os vértices do cluster>)

			sort(clusters.begin(), clusters.end());

			int i = 0;

			string solutionFileName = NAME+"(solution)";
			ofstream solution(solutionFileName+".txt");

			do {

				M.push_back(i++);

				for (int m = 0; i < M.size(); m++) {
					M[m].trajectory.push_back(searchById(0));
					M[m].position = searchById(0);
				}

				while (verifyAvailable()) {
					for (int v = 0; v < M.size(); v++) {
						Cluster *a = &closestClusterAvailable(M[v].position);
						concatenateTrajectory(M[v].trajectory, routing(M[v].position, *a));
						M[v].position = M[v].trajectory.back();
						M[v].t = routingTime(M[v].trajectory);
						a->available = false;
						sort(M.begin(), M.end());
					}
				}
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

			} while(!verifySolution(solutionFileName));
		}
		

		int routingTime(Node &origem, Node &destino) {
			return 0;
		}

		int routingTime(vector<Node>& trajectory) {
			return 0;
		}

		vector<Node> routing(Node &origem, Cluster &cluster) {
			vector<Node> route;
			for (int i = 0; i < cluster.vertices.size(); i++) {
				route.push_back(*cluster.vertices[i]);
			}
			return route;
		}

		int routingTime(Node &origem, Cluster &cluster) {
			// routing(origem, cluster);
			return 0;
		}

		Cluster& closestClusterAvailable(Node &origem) {
			for (int i = 0; i < clusters.size(); i++) {
                if (clusters[i].available)
                    return clusters[i];
            }
            throw ClusterNotFound();
		}

		void concatenateTrajectory(vector<Node>& tr1, vector<Node> tr2) {
			for (int i = 0; i < tr2.size(); i++)
				tr1.push_back(tr2[i]);
		}
		
		bool verifyAvailable() {
			for (int j = 0; j < clusters.size(); j++) {
				if(clusters[j].available) {
					return true;
				}
			}
			return false;
		}
		*/

        void imprimir() {
			for (int i = 0; i < (int)A.size(); i++) {
				cout << "[" << A[i].origem->id << "(" << A[i].origem->timeWindow.first << ")" << "," << A[i].destino->id << "(" << A[i].destino->timeWindow.first << ")" << "]->c(" << A[i].c << ")" << endl;
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