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
// #include "cfilterindex.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <chrono>

#include <cstdio>
// #include <faiss>
// #include "faiss/Clustering.h"
// #include "faiss/Index.h"
// #include "faiss/utils.h"//include something for map
using namespace std;

class PreFilterIndex
{
    public:
        PreFilterIndex(vector<vector<int>> data, vector<set<string>> properties, double& invert_time, double& invert_pair_time);
        vector<vector<int>> query(vector<vector<int>> queryset, vector<set<string>> queryprops, int num_results, int max_num_distances, double& filter_time, double& filter_pair_time, double& search_time);
        vector<int> findNearestNeighbor(vector<int> query, set<string> props, int num_results, int max_num_distances, double& filter_time, double& filter_pair_time, double& search_time);
        vector<int> satisfyingIDs(set<string> props);
        vector<int> satisfyingPairIDs(set<string> props);

        
        vector<vector<int>> dataset;
        map<string, set<int>> inverted_index;
        map<string, map<string, set<int>>> inverted_pair_index;
};