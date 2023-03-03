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
#include "FaissKMeans.h"
// #include "cfilterindex.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <chrono>

#include <cstdio>
#include "faiss/AutoTune.h"
#include "faiss/index_factory.h"
#include "faiss/index_io.h"
using namespace std;

class PostFilterIndex
{
    public:
        PostFilterIndex(vector<vector<int>> data, vector<set<string>> properties, int km_clusters, int km_itits, int km_iters, int km_learn);
        vector<vector<vector<int>>> query(vector<vector<int>> queryset, vector<set<string>> queryprops, int num_results, int num_probes);
        vector<vector<int>> split_clusters(vector<int> cluster_assignments, int num_clusters);
        
        FaissKMeans model;
        vector<int> data_ids;
        vector<vector<int>> cluster_bounds;
        vector<vector<int>> dataset;
        map<string, set<int>> inverted_index;
        vector<vector<int>> datdaset;
        vector<set<string>> properties;
};