/*
	Ranulfo Mascari Neto (10A) 202120702
	Heitor Rodrigues Sabino (10A) 202120101
*/
#include "libraries.h"
#include "node.h"
#include "cluster.h"
#include "edge.h"
#include "vehicle.h"
#include "graph.h"
#include "randomSolutionGenerator.h"
#include "readInstance.h"

int main() {

	string fileName = "instances/bar-n100-1-c";
	// cin >> fileName;

	Graph graph;
	readInstance(fileName, graph);
	//graph.groupByCluster();
	//graph.imprimirClusters();

	string solution = randomSolutionGenerator(graph.SIZE);
	graph.verifySolution(solution);

	//graph.verifySolution(graph.resolve());

	//graph.imprimir();


	return 0;
}