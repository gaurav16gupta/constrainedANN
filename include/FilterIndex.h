#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <set>
#include <iterator>
#include <stdlib.h>     
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

#include "cluster.h"
using namespace std;

class FilterIndex
{
    public:
        FilterIndex(float* data, size_t d_, size_t nb_, size_t nc_, vector<vector<string>>properties_, string algo, int mode);
        void get_index(string metric, string indexpath, int mode);
        void get_cluster_propertiesIndex();

        void loadIndex(string indexpath);
        void query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int num_mini_probes);
        void findNearestNeighbor(float* query, vector<string> Stprops, int num_results, int nprobe, size_t qnum);
        vector<uint32_t> satisfyingIDs(vector<uint16_t> props);
        void get_mc_propertiesIndex();
        // bool not_in(uint16_t x, vector<pair<uint16_t, pair<uint16_t, int>>> &maxMC);

        float *dataset; //use <dtype> array instead
        float *dataset_reordered;
        float *centroids; 
        float* cen_norms;
        float* data_norms;
        float *data_norms_reordered;
        uint32_t* invLookup;
        uint32_t* Lookup; 
        uint32_t* counts;
        int32_t* neighbor_set;
        int treelen;
        int numAttr;

        cluster* clusterAlgo; // parent class
        unordered_map<uint8_t, uint8_t>PrpAtrMap;
        vector<vector<uint8_t>>properties;
        uint8_t* properties_reordered;
        vector<vector<uint16_t>> ClusterProperties;//properties of each cluster
        // vector<pair<uint16_t, pair<uint16_t, int>>> maxMC;
        uint16_t* maxMC;

        uint32_t d, nb, nc, k;
        unordered_map<uint16_t, vector<uint32_t>> inverted_index; //can use other efficient maps
        unordered_map<string, uint16_t> prLook; 
};