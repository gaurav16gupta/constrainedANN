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
#include <unordered_map>
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
        PreFilterIndex(float* data, size_t d_, size_t nb_, size_t nq_, size_t k_, vector<vector<string>> properties);
        vector<vector<uint32_t>> query(float* queryset, vector<vector<string>> queryprops, int num_results, int max_num_distances);
        vector<uint32_t> findNearestNeighbor(float* query, vector<string> props, int num_results, int max_num_distances);
        vector<uint32_t> satisfyingIDs(vector<string> props);

        float* dataset; //use <dtype> array instead
        uint32_t d, nb, nq, k;
        unordered_map<string, vector<uint32_t>> inverted_index; //can use other efficient maps
};