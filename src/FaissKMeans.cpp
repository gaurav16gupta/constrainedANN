#include <vector>
#include <set>
#include "readfile.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <chrono>

#include <cstdio>

#include "utils.h"
#include "FaissKMeans.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include "faiss/AutoTune.h"
#include "faiss/index_factory.h"
#include "faiss/index_io.h"
#include "faiss/Clustering.h"
//Use quotations for a .h file

//include something for map
using namespace std;

//Use K Means Clustering to solve near neighbor
FaissKMeans::FaissKMeans(int num_clusters = 8, int num_init = 10, int max_iter = 300)
{
    num_clusters = num_clusters; //might not need this
    num_init = num_init;
    max_iter = max_iter;
}

float * FaissKMeans::flatten(vector<vector<int>> X)
{
    float * flatX = new float[X.size() * X[0].size()];
    int coord = 0;
    for (vector<int> point : X)
    {
        for (int coordinate : point)
        {
            flatX[coord] = (float) coordinate;
        }
    }
    return flatX;
}

void FaissKMeans::fit(vector<vector<int>> X)
{
    kmeans = faiss::index_factory(X[0].size(), "");
    float * Xarray = flatten(X);
    kmeans->train(X.size(), Xarray);
    //  float kmeans_clustering(size_t d, size_t n, size_t k, const float *x, float *centroids)
    float a = kmeans_clustering(X[0].size(), X.size(), num_clusters, const float *x, float *centroids)
}

int64_t * FaissKMeans::predict(vector<vector<int>> X, int topk)
{
    float * distances;
    int64_t *labels;
    kmeans->search(X.size(), flatten(X), topk, distances, labels);
    return labels;
}

vector<float> FaissKMeans::cluster_centers()
{
    return c_centers = kmeans->centroids;
}