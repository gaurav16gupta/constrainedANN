#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <set>
#include <iterator>
#include "MurmurHash3.h"
#include "cbloomfilter.h"
#include "crambo.h"
#include "readfile.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <chrono>
#include <cstdio>
#include "readfile.h"
#include "utils.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <sys/time.h>
#include "faiss/AutoTune.h"
#include "faiss/index_factory.h"
#include "faiss/index_io.h"
using namespace std;

class FaissKMeans
{
    public:
        FaissKMeans(int num_clusters, int num_init, int max_iter);
        float * flatten(vector<vector<int>> X);
        void fit(vector<vector<int>> X);
        int64_t * predict(vector<vector<int>> X, int topk = 1);
        vector<float> cluster_centers();

        int num_clusters;
        int num_init;
        int max_iter;
        faiss::Index * kmeans;
        vector<float> c_centers;
        double inertia;
};