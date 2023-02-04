#include "libraries.h"

int myrandom (int i) { return rand()%i;}  

string randomSolutionGenerator(int SIZE) { // Retorna o nome do arquivo de solução
	
	srand(time(NULL));
	string solutionFileName = "random";
	ofstream solution(solutionFileName+".txt");
	vector<int> nodes; // Conjunto de vértices do grafo
	int vehicle = 0; // <id> do veículo
	for (int i = 1; i < SIZE; i++) { // Depósito é desconsiderado
		nodes.push_back(i);
	}
	while(!nodes.empty()) {
		random_shuffle(nodes.begin(), nodes.end(), myrandom); // Embaralhar os vértices
		solution << vehicle << ";";
		vector<int> rota; // Rota do veículo
		rota.push_back(0); // Início no vértice 0 (depósito)
		for (int i = 0; i < nodes.size(); i++) {
			rota.push_back(nodes.back());
			nodes.pop_back();
		}
		rota.push_back(0); // Fim no vértice 0 (depósito)
		for (int i = 0; i < rota.size(); i++) {
			if(i == rota.size()-1)
				solution << rota[i];
			else
				solution << rota[i] << ",";

		}
		solution << endl;
		vehicle++; // Insere mais um veículo na frota da solucão
	}
	return solutionFileName;
}