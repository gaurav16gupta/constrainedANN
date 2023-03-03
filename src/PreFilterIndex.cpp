#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <random>
#include <sys/time.h>

#include "cbloomfilter.h"
#include "crambo.h"
#include "readfile.h"
#include "utils.h"
#include "PreFilterIndex.h"
//Use quotations for a .h file

using namespace std;

//inverted_index is an inverted default dictionary of properties that maps properties to the IDs that have them
PreFilterIndex::PreFilterIndex(float* data, size_t d_, size_t nb_, size_t nq_, size_t k_, vector<vector<string>> properties){
    dataset = data;
    d =d_;
    nb = nb_;
    nq = nq_;
    k = k_;
    for (int dataID = 0; dataID < properties.size(); dataID++){
        for (string property : properties[dataID]){
            inverted_index[property].push_back(dataID); //constraint:vectorNum
        }
    }
    // Sort each vector in ascending order. 
    for (auto v : inverted_index) {
        std::sort(v.second.begin(), v.second.end());
    }
}

vector<vector<uint32_t>> PreFilterIndex::query(float* queryset, vector<vector<string>> queryprops, int num_results, int max_num_distances)
{
    //queryset is an array of queries (*float), queryprops: query properties
    vector<vector<uint32_t>> neighbor_set;
    // string s;
    // cin>>s;
    for (size_t i = 0; i < nq; i++){
        cout<<i<<endl;
        neighbor_set.push_back(findNearestNeighbor(queryset+(i*d), queryprops[i], num_results, max_num_distances));
    }
    return neighbor_set;
}

vector<uint32_t> PreFilterIndex::findNearestNeighbor(float* query, vector<string> props, int num_results, int max_num_distances)
{
    // an intersection of all the IDs that have each property
    // vector<int> match_ids = satisfyingIDs(props);
    // for (string prop: props){
    //     cout<<prop<<",";
    // }
    // cout<<endl; 
    vector<float> topkDist;
    vector<uint32_t> match_ids = satisfyingIDs(props);
    int len_match_ids = match_ids.size();
    // int len_query = nq;
    //if there is not a data with these properties
    if (match_ids.empty()){
        cout<<"no matching attributes"<<endl;
        return match_ids;
    }
    //why cap it here?
    // if (max_num_distances < len_match_ids) {
    //     match_ids.resize(max_num_distances);
    //     match_ids.shrink_to_fit();
    //     len_match_ids = max_num_distances;
    // }
    if (len_match_ids <= num_results){
        // cout<<"len_match_ids <= num_results"<<endl;
        return match_ids;
    }
    else{
        return argTopK(query, dataset, d, nb, match_ids, len_match_ids, num_results, topkDist);
    }
}
// to be done by Jonah
vector<uint32_t> PreFilterIndex::satisfyingIDs(vector<string> props)
{
    // Should take O(set size)
    //use uint16 or uint8 to reduce the size
    vector<uint32_t> match_ids = inverted_index[props[0]];
    // map<uint32_t, uint8_t> array_ids; // need to init every time
    for (int i =1; i< props.size(); i++){ //loops over all properties
        // vector<uint32_t> valid_ids = ;
        std::vector<uint32_t> tmp;
        std::set_intersection(match_ids.begin(), match_ids.end(),  // Input iterators for first range 
                              inverted_index[props[i]].begin(), inverted_index[props[i]].end(), // Input iterators for second range 
                              std::back_inserter(tmp));            // Output iterator

        match_ids = tmp;   // Update the current result with the intersection of this iteration. 
        if (match_ids.empty()) {   // If there is no intersection, we can stop here as any further intersections will be empty too. 
            break;  
        }    					

        // for (uint32_t valid_id: match_ids){
        //     cout<<valid_id<<",";
        // }
        // cout<<endl; 

        // if (i< num_props-1){
        //     for (uint32_t id : valid_ids){ //items of a given property
        //         array_ids[id] += 1; // key: item, value: # properties it satisfies
        //     }
        // }
        // else {
        //     for (uint32_t id : valid_ids){
        //         if (array_ids[id] == num_props-2)  match_ids.push_back(id); 
        //     }
        // }
    }
    
    return match_ids;
}

int main()
{
    cout << "PreFilter running..." << endl;

    size_t d2, nb,nq; 
    // float* xt = fvecs_read("../data/sift/sift/sift_learn.fvecs", &d, &nt); // not needed now
    float* data = fvecs_read("../data/sift/sift/sift_base.fvecs", &d2, &nb);
    // for (int j=0;j<10;j++){
    //     for (int i=0;i<128;i++){
    //         cout<<data[i+j*128]<<' ';
    //     }
    //     cout<<endl;
    // }
    vector<vector<string>> properties = getproperties("../data/sift/sift_label/label_sift_base.txt",' ');
    cout<<properties.size()<<endl;
    cout << "Data files read" << endl;
    PreFilterIndex prefilter(data, 128, 1000000, 4, 10, properties);
    cout << "Indexed" << endl;

    float* queryset = fvecs_read("../data/sift/sift/sift_query.fvecs", &d2, &nq);
    vector<vector<string>> queryprops = getproperties("../data/sift/sift_label/label_sift_query.txt",' ');
    cout << "Query files read..." << endl;
    vector<vector<uint32_t>> query = prefilter.query(queryset, queryprops, 10, 300);

    cout << "Querying worked..." << endl;
    for (int coord : query[0]){
        cout << coord << " ";
    }
    cout<<endl;
    //load groundtruth
    int* queryGT = ivecs_read("../data/sift/sift/label_sift_hard_groundtruth.ivecs", &d2, &nq);
    for (int i; i<10; i++){
        cout<<queryGT[i]<<" ";
    }


    // vector<vector<int>> answer = computeGroundTruth(queryset, queryprops, data, properties, 10);// should be precomputed and stored
    // cout << "Proper answer found..." << endl;
    // for (int coord : answer[4])
    // {
    //     cout << coord << endl;
    // }
    // double accuracy = computeRecall(answer, query);
    // cout << "accuracy:" << accuracy << endl;

     // vector<vector<int>> data = coordinates("./data/points_coords_file.txt");
    // vector<vector<int>> queryset = coordinates("./data/query_coords_file.txt");
}

