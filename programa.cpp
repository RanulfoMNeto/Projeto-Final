/*
	Ranulfo Mascari Neto (10A) 202120702
	Heitor Rodrigues Sabino (10A) 202120101
*/
#include "libraries.h"
#include "node.h"
#include "edge.h"
#include "vehicle.h"
#include "graph.h"
#include "randomSolutionGenerator.h"
#include "readInstance.h"

int main() {

	string fileName = "instances/bar-test";
	// cin >> fileName;

	Graph grafo;
	readInstance(fileName, grafo);

	// grafo.imprimir();
	//grafo.imprimirClusters();
	//grafo.resolve();

	string solution = randomSolutionGenerator(grafo.SIZE);
	ifstream solutionFile(solution+".txt");
	grafo.verifySolution(solution);

	return 0;
}