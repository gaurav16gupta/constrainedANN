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
#include "cfilterindex.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
//#include <boost/range/combine.hpp>

#include <cstdio>
#include <faiss>
#include "faiss/Clustering.h"
#include "faiss/Index.h"
#include "faiss/utils.h"//include something for map
using namespace std;

class FilterIndex
{
    public:
        FilterIndex(vector<vector<int>> data, vector<set<string>> properties, int km_clusters, int km_itits, int km_iters, int km_learn, int mini_size, int rambo_buckets, int rambo_reps);
        int create_rambo_filter(int rambo_buckets, int rambo_reps, vector<set<string>> properties, vector<int> mini_clusters);
        vector<int> split_mini_clusters(vector<vector<int>> cluster_assignments, int mini_size);
        tuple<int64_t *, vector<float>> create_kmeans_index(vector<vector<int>> data, int km_clusters, int km_inits, int km_iters, int km_learn);
        vector<vector<int>> query(vector<vector<int>> queryset, vector<set<string>> queryprops, int num_results, int num_mini_probes);

        FaissKMeans model;
        vector<int> data_ids;
        vector<vector<int>> cluster_bounds;
        vector<vector<int>> dataset;
        map<string, set<int>> inverted_index;
        vector<vector<int>> dataset;
        vector<set<string>> properties;
};