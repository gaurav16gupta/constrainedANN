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

class FilterIndex
{
    public:
        FilterIndex(float* data, size_t d_, size_t nb_, size_t nc_, size_t k_, vector<vector<string>>properties_);
        void get_kmeans_index(string metric);
        void get_cluster_propertiesIndex();
        // void create_Inv_filter(vector<vector<string>> ClusterProperties);
        // void create_rambo_filter(int B, int R, vector<vector<string>> ClusterProperties, float* miniClusters);
        // float PartialL2(float* a, float* b, float* b_norm, uint32_t id, float dist);
        float L2(float* a, float* b, uint32_t id);
        vector<vector<uint32_t>> query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int num_mini_probes, int m);
        vector<uint32_t> findNearestNeighbor(float* query, vector<string> Stprops, int num_results, int max_num_distances, int m);
        vector<uint32_t> satisfyingIDs(vector<uint16_t> props);

        float *dataset; //use <dtype> array instead
        float *centroids; 
        float* cen_norms;
        float* data_norms;
        uint32_t* invLookup;
        uint32_t* Lookup; 
        uint32_t* counts;
        vector<vector<uint16_t>>properties;
        vector<vector<uint16_t>> ClusterProperties;//properties of each cluster
        uint32_t d, nb, nc, k;
        unordered_map<uint16_t, vector<uint32_t>> inverted_index; //can use other efficient maps
        unordered_map<string, uint16_t> prLook; 

};