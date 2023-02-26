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

void readBenchmarks(string fileName, vector<Instance> &instances) {

	ifstream instance(fileName+".txt");
	string line;

	getline(instance, line);

	while(getline(instance, line)) {
		stringstream sst(line);
		string info;
		int i = 0;
		string name;
		int M, C, clusters;
		while(getline(sst, info, ',')) {
			if (i == 0) {
				name = info;
			}
			if (i == 1) {
				M = stoi(info);
			}
			if (i == 2) {
				C = stoi(info);
			}
			if (i == 3) {
				clusters = stoi(info);
			}
			i++;
		}

		Instance instance(name, M, C, clusters);
		instances.push_back(instance);
	}
}

int main() {

	string benchmarksFileName = "benchmarks";
	vector<Instance> instances;
	readBenchmarks(benchmarksFileName, instances);
	ofstream report("report.csv"); // Performance report file
	report << "name,M,C,MX,CX,clusters" << endl;
	/*
		[Informações da instância]
		name: Nome da instância
		M: Número de veículos gastos pela melhor solução conhecida
        C: Custo gasto pelos veículos da melhor solução conhecida
		MX: Número de veículos gastos pela solução gerada
		CX: Custo gasto pelos veículos da solução gerada
		clusters: Número de clusters da solução gerada
	*/

	double sum = 0;
	for (int i = 0; i < instances.size(); i++) {
		string fileName = "instances/";
		Graph graph;
		instances[i].graph = &graph;

		string instanceName = instances[i].name;
		fileName += instanceName;
		cout << "Instance name: " << fileName << endl;

		cout << "Reading..." << endl;
		readInstance(fileName, graph);

		//graph.toCSV(); // File used to find distortion Score Elbow in Google Colab

		cout << "Clustering..." << endl;
		graph.clustering(instances[i].clusters);

		cout << "Solving..." << endl;
		graph.solve();

		report << instances[i].name << "," << instances[i].M << "," << instances[i].C << "," << graph.M.size() << "," << graph.totalRoutingTime() << "," << graph.clusters.size() << endl; // File for statistical analysis
		sum += graph.M.size();
		instances[i].calculateDeviation();
	}


	/*
		Desvio da solução() = (MX-M)/M * 100; CX-C)/C * 100)
	*/

	cout << endl << "[Desvio das soluções] " << endl << endl;
	
	double average = sum/instances.size();
	double improvement = 100-(average*100)/81.0333; // Média resultante da solução com instâncias sem cluster: 81.0333
	cout << "Melhoria (em relação a solução sem cluster): " << improvement << "%" << endl;
	
	for (int i = 0; i < instances.size(); i++) {
		cout << instances[i].name << ": (" << instances[i].deviation.first << " ; " << instances[i].deviation.second << ")" << endl; 
	}

	return 0;
}
