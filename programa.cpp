/*
	Ranulfo Mascari Neto (10A) 202120702
	Heitor Rodrigues Sabino (10A) 202120101
*/
#include "libraries.h"
#include "node.h"
#include "cluster.h"
#include "edge.h"
#include "vehicle.h"
#include "kMeansClustering.h"
#include "graph.h"
#include "randomSolutionGenerator.h"
#include "readInstance.h"

int main() {

	string fileName = "instances/nyc-n200-4";
	// cin >> fileName;

	int numVehicle;

	Graph graph;
	readInstance(fileName, graph);
	bool solved;

	do {
		graph.clear();
		graph.clustering();
		solved = graph.resolve();
	} while(!solved);

	graph.imprimirClustersById();
	graph.write();

	return 0;
}
