#include "libraries.h"

class Graph {
	public:
		int SIZE, CAPACITY, ROUTE_TIME, CLUSTERS;
        vector<Node> V; // Conjunto de vértices (Coleta U Entrega U Depósito)
		vector<Edge> A;
		vector<Vehicle> M;
		vector<vector<int> > clusters;

		Graph() {
			CLUSTERS = 0;
		}

        void adicionarVertice(tipo id, string lat, string lon, int dem, int etw, int ltw, int dur, tipo p, tipo d, int cluster) {
            Node vertice(id, lat, lon, dem, etw, ltw, dur, p, d, cluster);
			while (clusters.size() <= cluster) {
				vector<int> aux;
				clusters.push_back(aux);
				CLUSTERS++;
			}
            V.push_back(vertice);
			clusters[cluster].push_back(id); // Insere o vértice no cluster
			// Cluster 0 -> Vértices sem cluster (pontos de coleta e origem)
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

		/* 
		// Algoritmo de resolução (desenvolvimento)
		void resolve() {
			vector<int> trajectory;

			vector<int> coleta;
			int menor;
			for (int m = 0; m < 1; m++) {
				Vehicle veiculo(m);
				M.push_back(veiculo);
				for (int i = 0; i < clusters.size(); i++) {
					for (int j = 0; j < clusters[i].size(); j++) {
						coleta.push_back(searchById(clusters[i][j]).p);
					}
					sort(coleta.begin(), coleta.end());
					while(!coleta.empty()) {
						coleta.pop_back();
					}

				}
			}
		}
		*/

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

        void imprimir() {
			for (int i = 0; i < (int)A.size(); i++) {
				cout << "[" << A[i].origem->id << "(" << A[i].origem->timeWindow.first << ")" << "," << A[i].destino->id << "(" << A[i].destino->timeWindow.first << ")" << "]->c(" << A[i].c << ")" << endl;
			}
		}

		void imprimirClusters() {
			cout << "== " << CLUSTERS << " ==" << endl;
			for (int i = 0; i < clusters.size(); i++) {
				cout << "Cluster: " << i << endl;
				for (int j = 0; j < clusters[i].size(); j++) {
					cout << searchById(clusters[i][j]).id << endl;
				}
				cout << endl;
			}
		}

};