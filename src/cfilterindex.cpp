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
#include "readfile.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>

#include <cstdio>
#include <faiss>
#include "faiss/Clustering.h"
#include "faiss/Index.h"
#include "faiss/utils.h"
//Use quotations for a .h file

//include something for map
using namespace std;

//Use K Means Clustering to solve near neighbor (should be easy :D)
FaissKMeans::FaissKMeans(int num_clusters = 8, int num_init = 10, int max_iter = 300)
{
    this->num_clusters = num_clusters; //might not need this
    this->num_init = num_init;
    this->max_iter = max_iter;
}

void FaissKMeans::fit(vector<float> X, int num_vectors, int num_dimensions)
{
    faiss::ClusteringParameters cp;
    cp.niter = max_iter;
    cp.nredo = num_init;
    faiss::Clustering this->kmeans(num_dimensions, num_clusters, cp);
    faiss::Index this->index(num_dimensions);
    kmeans.train(num_vectors, X.data(), index);
    cluster_centers = kmeans.centroids;
    inertia = void; //What do I assign here?
}

vector<int> FaissKMeans::predict(vector<float> X, int num_vectors, int num_dimensions, int topk = 1)
{
    vector<int> labels;
    vector<float> distances;
    //check distances
    index.search(num_vectors, X.data(), topk, distances, labels);
    return labels;
}

vector<float> FaissKMeans::cluster_centers()
{
    return cluster_centers;
}

//dataset is the set of data given
//inverted_index is an inverted default dictionary of properties that maps properties to the IDs that have them
PreFilterIndex::PreFilterIndex(vector<vector<int>> data, map<int, set<int>> properties, int dataLength) //<int, vector<int>>
{
    dataset = data;
    len_dataset = dataset.size();
    for (auto const& [dataID, val] : properties)
    {
        for (int property : val)
        {
            inverted_index[property].insert(dataID);
        }
    }
}

vector<vector<int>> PreFilterIndex::query(vector<vector<int>> queryset, vector<set<int>> queryprops, int num_results, int max_num_distances, int& num_distances)
{
    //queries are *int values of length len_query
    //queryset is an array of queries (**int) and has a length of len_queryset
    num_distances = 0;
    vector<vector<int>> neighbor_set;
    int len_queryset = queryset.size();
    for (int i = 0; i < len_queryset; i++)
    {
        findNearestNeighbor(queryset[i], queryprops[i], neighbor_set[i], num_results, max_num_distances, num_distances);
    }
    return neighbor_set;
}

void PreFilterIndex::findNearestNeighbor(vector<int> query, set<int> props, vector<int>& neighbor, int num_results, int max_num_distances, int& num_distances)
{
    //use stdvector instead of int** if you want=
    //make a helper function to find all the match_ids
    //essentially an intersection of all the IDs that have each property
    // tuple<std::vector<int>, int> return_values = satisfyingIDs(props); //use the address instead of making a copy
    // std::vector<int> match_ids = get<0>(return_values); //adjust satisfyingIDs appropriately
    vector<int> match_ids = satisfyingIDs(props);
    int len_match_ids = match_ids.size();
    int len_query = query.size();
    // int len_match_ids = get<1>(return_value);
    if (match_ids.empty()) //if there is not a data with these properties
    {
        return;
    }

    if (max_num_distances < len_match_ids) //not null
    {
        match_ids.resize(max_num_distances);
        match_ids.shrink_to_fit();
        len_match_ids = max_num_distances;
    }

    if (len_match_ids <= num_results)
    {
        neighbor = match_ids;
    }
    else
    {
        vector<tuple<int, double>> top_k;
        for (int x = 0; x < num_results; x ++)
        {
            top_k.push_back({x, -1.0});
        }
        for (int index = 0; index < len_match_ids; index++)
        {
            double dist = 0;
            int id = match_ids[index];
            for (int coord = 0; coord < len_query; coord++)
            {
                dist += pow(dataset[id][coord] - query[coord], 2);
            }
            dist = pow(dist, 0.5); 
            for (int order = 0; order < num_results; order++)
            {
                if (dist < get<1>(top_k.at(order)) || get<1>(top_k.at(order)) < 0)
                {
                    top_k.insert(top_k.begin() + order, {id, dist});
                    top_k.pop_back();
                    continue;
                }
            }
        }
        vector<int> nearest_neighbors;
        for (int index = 0; index < num_results; index++)
        {
            nearest_neighbors.push_back(get<0>(top_k.at(index)));
        }
        neighbor = nearest_neighbors;
    }
    num_distances += len_match_ids;
}

vector<int> PreFilterIndex::satisfyingIDs(set<int> props)
{
    // Should take O(set size)
    //use uint16 or uint8 to reduce the size
    map<int, int> array_ids;
    int num_props = props.size();
    for (int property : props)
    {
        set<int> valid_ids = inverted_index[property];
        for (int id : valid_ids)
        {
            array_ids[id] += 1;
        }
    }
    vector<int> match_ids;
    for (auto const& [index, freq] : array_ids)
    {
        if (freq >= num_props)
        {
            match_ids.push_back(index);
        }
    }
    return match_ids;
}

PostFilterIndex::PostFilterIndex(vector<vector<int>> data, map<int, set<int>> properties, int km_clusters = -1, int km_itits = 1, int km_iters = 10, int km_learn = -1)
{
    if (km_clusters == -1)
    {
        km_clusters = max(data.size() / 2000, 1);
    }
    this->model = new FaissKMeans(km_clusters, km_itits, km_iters);
    auto first = data.cbegin();
    auto last = data.cbegin() + km_learn + 1;
    vector<vector<int>> learn(first, last);
    if (km_learn == -1)
    {
        learn = data;
    }
    model.fit(learn);
    vector<int> cluster_assignments = model.predict(data);
    vector<int> sorted_order = argsort(cluster_assignments); //This should also alter cluster_assignments
    vector<int> new_cluster_assignments; //Make this an array
    new_cluster_assignments.resize(cluster_assignments.size());
    for (int i = 0; i < cluster_assignments.size(); i++)
    {
        new_cluster_assignments[sorted_order[i]] = cluster_assignments[i];
    }
    cluster_assignments = new_cluster_assignments;
    vector<int> data_ids;
    vector<int> new_data_ids;
    for (int i = 0; i < data.size(); i++)
    {
        data_ids.add(i);
    }
    vector<int> new_data_ids;
    new_data_ids.resize(data_ids.size());
    for (int i = 0; i < data_ids.size(); i++)
    {
        new_data_ids[sorted_order[i]] = data_ids[i];
    }
    data_ids = new_data_ids; //How to do this in C++
    this-> cluster_bounds = split_clusters(cluster_assignments, km_clusters);
    vector<int> new_dataset;
    new_dataset.resize(dataset.size());
    for (int i = 0; i < dataset.size(); i++)
    {
        new_dataset[sorted_order[i]] = dataset[i];
    }
    this-> dataset = new_dataset; //This too
    this-> properties = properties;
    this-> data_ids = data_ids;
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
            vector<int> mid_id = cluster_assignments[mid];
            if (mid_id <= cluter_id)
            {
                low = mid + 1;
            }
            else if (mid_id > cluster_id)
            {
                high = mid;
            }
        }
        int bound = low + (high - low) / 2;
        cluster_bounds[cluster_id][1] = bound;
        cluster_bounds[cluster_id + 1][0] = bound;
    }
    return cluster_bounds;
}

vector<vector<vector<int>>> PostFilterIndex::query(vector<vector<int>> queryset, vector<set<int>> queryprops, int num_results, int num_probes = 1, int& num_distances)
{
    int distances = 0;
    vector<vector<vector<int>>> neighbor_set;
    vector<vector<int>> queryclusters = model.predict(queryset, num_probes);
    for (int i = 0; i < queryset.size(); i++)
    {
        vector<int> query = queryset[i];
        vector<int> clusters = queryclusters[i];
        set<int> prop = queryprops[i];
        vector<vector<int>> candidate_ids;
        vector<double> candidate_dists;
        for (int cluster_id : clusters)
        {
            int lower = cluster_bounds[cluster_id][0];
            int upper = cluster_bounds[cluster_id][1];
            auto first = data_ids.cbegin() + lower;
            auto last = data_ids.cbegin() + upper + 1;
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
            vector<int> sorted_order = candidate_dists.argsort();
            vector<int> new_candidate_ids;
            new_candidate_ids.resize(candidate_dists.size());
            for (int i = 0; i < candidate_dists.size(); i++)
            {
                new_candidate_ids[sorted_order[i]] = candidate_ids[i];
            }
            candidate_ids = new_candidate_ids;

        }
        neighbor_set.push_back(nearest_neighbors);
    }
    return neighbor_set;
}

//This was pulled
template<typename T>
std::vector<size_t> argsort(const std::vector<T> &array) {
    std::vector<size_t> indices(array.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
              [&array](int left, int right) -> bool {
                  // sort indices according to corresponding array element
                  return array[left] < array[right];
              });

    return indices;
}