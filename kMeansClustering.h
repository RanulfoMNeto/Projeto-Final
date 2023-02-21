#include "libraries.h"

/*
    Algoritmo baseado na implementação desenvolvida por Robert Andrew Martin de Reasonable Deviations.
    Implementing k-means clustering from scratch in C++: https://reasonabledeviations.com/2019/10/02/k-means-in-cpp/
*/

using namespace std;

int myrandom2 (int i) { return rand()%i;} 

void kMeansClustering(vector<Node*> &points, int k) {
    int n = points.size(); // the number of nodes
    vector<int> rdm;
    srand(time(0));  // need to set the random seed
    for (int i = 0; i < n; i++)
        rdm.push_back(i);
    random_shuffle(rdm.begin(), rdm.end(), myrandom2);
    vector<Node> centroids;
    for (int i = 0; i < k; ++i) {
        centroids.push_back(*points[rdm.back()]);
        //cout << rdm.back() << "->" << points[rdm.back()]->id << endl;
        rdm.pop_back();
    }
    int cot = 0;
    for (vector<Node>::iterator c = begin(centroids); c != end(centroids); ++c) {
        // quick hack to get cluster index
        int clusterId = c - begin(centroids);


        for (int d = 0; d < points.size(); d++) {
                
            Node *p = points[d];
            double dist = c->euclideanDistance(*p);
            if (dist < p->minDist) {
                p->minDist = dist;
                p->cluster = clusterId;
            }
            points[d] = p;
        }
    }

    vector<int> nPoints;
    vector<double> sumX, sumY;

    // Initialise with zeroes
    for (int j = 0; j < k; ++j) {
        nPoints.push_back(0);
        sumX.push_back(0.0);
        sumY.push_back(0.0);
    }

    // Iterate over points to append data to centroids
    for (vector<Node*>::iterator it = points.begin(); it != points.end(); ++it) {
        int clusterId = (*it)->cluster;
        nPoints[clusterId] += 1;
        sumX[clusterId] += stof((*it)->lat);
        sumY[clusterId] += stof((*it)->lon);

        (*it)->minDist = numeric_limits<double>::max();  // reset euclideanDistance
    }

    // Compute the new centroids
    for (vector<Node>::iterator c = begin(centroids); c != end(centroids); ++c) {
        int clusterId = c - begin(centroids);
        c->lat = sumX[clusterId] / nPoints[clusterId];
        c->lat = sumY[clusterId] / nPoints[clusterId];
    }

}