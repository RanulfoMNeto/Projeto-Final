/*
	Ranulfo Mascari Neto (10A) 202120702
	Heitor Rodrigues Sabino (10A) 202120101
*/
#include "libraries.h"
#include "node.h"
#include "cluster.h"
#include "edge.h"
#include "vehicle.h"
#include "randomSolutionGenerator.h"
#include "kMeansClustering.h"
#include "graph.h"
#include "readInstance.h"
#include "instance.h"

int main() {

	string fileName = "instances/";
	Graph graph;

	string instanceName;
	cout << "Instance name: " << fileName;
	cin >> instanceName;

	fileName += instanceName;

	cout << "Reading..." << endl;
	readInstance(fileName, graph);

	int number = numeric_limits<int>::max();
	while(number > (graph.SIZE/2)) {
		cout << "Number of cluster (1 at |V|/2): ";
		cin >> number;
	}

	cout << "Clustering..." << endl;
	graph.clustering(number);

	cout << "Solving..." << endl;
	graph.solve();

	graph.myMaps();

	return 0;
}
