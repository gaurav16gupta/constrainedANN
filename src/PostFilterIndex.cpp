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
#include "readfile.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <chrono>

#include <cmath>
#include <cstdio>

#include "readfile.h"
#include "utils.h"
#include "PostFilterIndex.h"
#include "FaissKMeans.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <sys/time.h>
#include "faiss/AutoTune.h"
#include "faiss/index_factory.h"
#include "faiss/index_io.h"
//Use quotations for a .h file

//include something for map
using namespace std;

PostFilterIndex::PostFilterIndex(vector<vector<int>> data, vector<set<string>> properties, int km_clusters = -1, int km_itits = 1, int km_iters = 10, int km_learn = -1)
{
    if (km_clusters == -1)
    {
        km_clusters = (data.size() / 2000 > 1) ? data.size() / 2000 : 1;
    }
    this->model = FaissKMeans(km_clusters, km_itits, km_iters);
    vector<vector<int>> learnset = {data.begin(), data.begin() + km_learn};
    if (km_learn == -1)
    {
        learnset = data;
    }
    model.fit(learnset);
    int* cluster_assignments = (int*) model.predict(data); // datapt -> clust ID
    Utils utility;
    vector<int> sorted_order = utility.argsort(cluster_assignments, data.size()); 
    data_ids = sorted_order;
    int64_t* sorted_cluster_assignments = new int64_t[data.size()];
    for (int x = 0; x < data.size(); x++)
    {
        sorted_cluster_assignments[x] = cluster_assignments[sorted_order[x]]; 
        this->dataset.push_back(data[sorted_order[x]]);
    }
    cluster_assignments = (int *)sorted_cluster_assignments;
    vector<int> cluster_assignments_vector;
    for (int x = 0; x < data.size(); x++)
    {
       cluster_assignments_vector.push_back(cluster_assignments[x]);
    }
    cluster_bounds = split_clusters(cluster_assignments_vector, km_clusters);
    this->properties = properties;
} 

vector<vector<int>> PostFilterIndex::split_clusters(vector<int> cluster_assignments, int num_clusters)
{
    vector<vector<int>> cluster_bounds;
    for (int i = 0; i< num_clusters; i++)
    {
        vector<int> insert;
        insert.push_back(0);
        insert.push_back(cluster_assignments.size());
        cluster_bounds.push_back(insert);
    }
    for (int cluster_id = 0; cluster_id < num_clusters - 1; cluster_id++)
    {
        int low = cluster_bounds[cluster_id][0];
        int high = cluster_assignments.size();
        while (low < high)
        {
            int mid = low + (high - low) / 2;
            int mid_id = cluster_assignments[mid];
            if (mid_id <= cluster_id)
            {
                low = mid + 1;
            }
            else if (mid_id > cluster_id)
            {
                high = mid;
            }
        }
        int bound = low + (high - low) / 2;
        cluster_bounds[cluster_id][1] = bound; // |0-----|5----|9----------|------|
        cluster_bounds[cluster_id + 1][0] = bound;
    }
    return cluster_bounds;
}

vector<vector<vector<int>>> query(vector<vector<int>> queryset, vector<set<string>> queryprops, int num_results, int num_probes = 1)
{
    int distances = 0;
    vector<vector<vector<int>>> neighbor_set;
    int64_t * queryclusterarray = this->model.predict(queryset, num_probes);
    vector<vector<int>> queryclusters;
    for (int x = 0; x < queryset.size(); x++)
    {
        vector<int> cluster;
        for (int y = 0; y < num_probes; y++)
        {
            cluster.push_back(queryclusterarray[x * num_probes + y]);
        }
        queryclusters.push_back(cluster);
    }
    for (int i = 0; i < queryset.size(); i++)
    {
        vector<int> query = queryset[i];
        vector<int> clusters = queryclusters[i];
        set<string> prop = queryprops[i];
        vector<vector<int>> candidate_ids;
        vector<double> candidate_dists;
        for (int cluster_id : clusters)
        {
            int lower = this->cluster_bounds[cluster_id][0];
            int upper = this->cluster_bounds[cluster_id][1];
            auto first = this->data_ids.cbegin() + lower;
            auto last = this->data_ids.cbegin() + upper;
            vector<int> cluster_ids(first, last);
            double cluster_dists = 0;
            for (int index = 0; index < query.size(); index++)
            {
                cluster_dists += pow(cluster_ids[index] - query[index], 2);
            }
            cluster_dists = pow(cluster_dists, 0.5);
            candidate_ids.push_back(cluster_ids);
            candidate_dists.push_back(cluster_dists);
        }
        vector<vector<int>> nearest_neighbors;
        if (candidate_ids.size() <= num_results)
        {
            nearest_neighbors = candidate_ids;
        }
        else
        {
            Utils utility;
            vector<int> sorted_order = utility.vectorArgsort(candidate_dists);
            vector<vector<int>> new_candidate_ids;
            for (int i = 0; i < candidate_dists.size(); i++)
            {
                new_candidate_ids.push_back(candidate_ids[sorted_order[i]]);
            }
            candidate_ids = new_candidate_ids;

        }
        neighbor_set.push_back(nearest_neighbors);
    }
    return neighbor_set;
}
