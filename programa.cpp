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
//#include "kMeansClusteringKaggle.h"
#include "graph.h"
#include "randomSolutionGenerator.h"
#include "readInstance.h"
#include "instance.h"

int main() {

	string fileName = "instances/bar-n100-1";
	Graph graph;

	readInstance(fileName, graph);
	int number = 7;
	cout << "Number of cluster: ";
	//cin >> number;
	cout << "Clustering..." << endl;
	graph.clustering(number);

	cout << "Solving..." << endl;
	graph.solve();

	//graph.myMaps();

	return 0;
}
