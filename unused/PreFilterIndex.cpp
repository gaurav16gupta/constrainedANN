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

#include <cstdio>
// #include <faiss>
// #include "faiss/Clustering.h"
// #include "faiss/Index.h"
// #include "faiss/utils.h"
#include "readfile.h"
#include "utils.h"
#include "PreFilterIndex2.h"
//Use quotations for a .h file

//include something for map
using namespace std;

//dataset is the set of data given
//inverted_index is an inverted default dictionary of properties that maps properties to the IDs that have them
PreFilterIndex::PreFilterIndex(vector<vector<int>> data, vector<set<string>> properties, double& invert_time, double& invert_pair_time)
{
    this->dataset = data;
    auto start_train = std::chrono::high_resolution_clock::now();
    for (int dataID = 0; dataID < properties.size(); dataID++)
    {
        for (string property : properties[dataID])
        {
            this->inverted_index[property].insert(dataID);
        }
    }
    auto stop_train = std::chrono::high_resolution_clock::now();
    chrono::duration<double> time_took = stop_train - start_train;
    invert_time += time_took.count();

    auto start_train_pair = std::chrono::high_resolution_clock::now();
    for (int dataID = 0; dataID < properties.size(); dataID++)
    {
        for (string property1 : properties[dataID])
        {
            for (string property2 : properties[dataID])
            {
                this->inverted_pair_index[property1][property2].insert(dataID);
            }
        }
    }
    auto stop_train_pair = std::chrono::high_resolution_clock::now();
    chrono::duration<double> time_took_pair = stop_train_pair - start_train_pair;
    invert_pair_time += time_took_pair.count();
}

vector<vector<int>> PreFilterIndex::query(vector<vector<int>> queryset, vector<set<string>> queryprops, int num_results, int max_num_distances, double& filter_time, double& filter_pair_time, double& search_time)
{
    //queries are *int values of length len_query
    //queryset is an array of queries (**int) and has a length of len_queryset
    vector<vector<int>> neighbor_set;
    for (int i = 0; i < queryset.size(); i++)
    {
        neighbor_set.push_back(findNearestNeighbor(queryset[i], queryprops[i], num_results, max_num_distances, filter_time, filter_pair_time, search_time));
    }
    return neighbor_set;
}

vector<int> PreFilterIndex::findNearestNeighbor(vector<int> query, set<string> props, int num_results, int max_num_distances, double& filter_time, double& filter_pair_time, double& search_time)
{
    //use stdvector instead of int** if you want=
    //make a helper function to find all the match_ids
    //essentially an intersection of all the IDs that have each property
    // tuple<std::vector<int>, int> return_values = satisfyingIDs(props); //use the address instead of making a copy
    // std::vector<int> match_ids = get<0>(return_values); //adjust satisfyingIDs appropriately
    auto start_train = std::chrono::high_resolution_clock::now();
    vector<int> match_ids = satisfyingIDs(props);
    auto stop_train = std::chrono::high_resolution_clock::now();
    chrono::duration<double> time_took = stop_train - start_train;
    filter_time += time_took.count();

    auto start_train_pair = std::chrono::high_resolution_clock::now();
    vector<int> match_pair_ids = satisfyingPairIDs(props);
    auto stop_train_pair = std::chrono::high_resolution_clock::now();
    chrono::duration<double> time_took_pair = stop_train_pair - start_train_pair;
    filter_pair_time += time_took_pair.count();
    
    int len_match_ids = match_ids.size();
    // int len_match_ids = get<1>(return_value);
    if (match_ids.empty()) //if there is not a data with these properties
    {
        return match_ids;
    }

    if (max_num_distances < len_match_ids) //not null
    {
        match_ids.resize(max_num_distances);
        match_ids.shrink_to_fit();
        len_match_ids = max_num_distances;
    }

    auto start_train2 = std::chrono::high_resolution_clock::now();
    if (len_match_ids <= num_results)
    {
        vector<int> ans = match_ids;
        auto stop_train2 = std::chrono::high_resolution_clock::now();
	    chrono::duration<double> time_took2 = stop_train2 - start_train2;
        search_time += time_took2.count();
        return ans;
    }
    else
    {
        Utils utility;
        vector<int> ans = utility.argMatchTopK(query, dataset, num_results, match_ids);
        auto stop_train2 = std::chrono::high_resolution_clock::now();
	    chrono::duration<double> time_took2 = stop_train2 - start_train2;
        search_time += time_took2.count();
        return ans;
    }
    
}

vector<int> PreFilterIndex::satisfyingIDs(set<string> props)
{
    // Should take O(set size)
    //use uint16 or uint8 to reduce the size
    map<int, int> array_ids; //Replaced with another unordered map which is a faster way to do it
    int num_props = props.size();
    for (string property : props)
    {
        set<int> valid_ids = inverted_index[property];
        for (int id : valid_ids)
        {
            array_ids[id] += 1;
        }
    }
    vector<int> match_ids;
    for (auto pair : array_ids) //constant
    {
        //Not needed for single properties
        //In multiple properties this is fine
        if (pair.second >= num_props) //Make sense with multiple properties because intersection
        {
            match_ids.push_back(pair.first);
        }
    }
    return match_ids;
}

vector<int> PreFilterIndex::satisfyingPairIDs(set<string> props)
{
    vector<string> vector_props;
    set<int> valid_ids;
    for (string property1 : props)
    {
        for (string property2 : props)
        {
            if (property1.compare(property2) != 0)
            {
                valid_ids = inverted_pair_index[property1][property2];
            }
        }
    }
    vector<int> match_ids;
    for (int id : valid_ids)
    {
        match_ids.push_back(id);
    }
    return match_ids;
}

int main()
{
    ofstream fout;
    fout.open("./data/output_test.txt");
    
    cout << "PreFilter running..." << endl;
    ReadFile fileReader;
    vector<vector<int>> data = fileReader.coordinates("./data/points_coords_file.txt");
    vector<set<string>> properties = fileReader.properties("./data/point_properties_file.txt");
    data.resize(10000);
    data.shrink_to_fit();
    properties.resize(10000);
    properties.shrink_to_fit();
    cout << "Data files read..." << endl;

    double invert_time = 0;
    double invert_pair_time = 0;
    PreFilterIndex prefilter(data, properties, invert_time, invert_pair_time);
    cout << "Construction worked..." << endl;
    cout << "Construction took: " << invert_time << " seconds" << endl;
    fout << "Construction took: " << invert_time << " seconds" << endl;
    cout << "Construction pairs took: " << invert_pair_time << " seconds" << endl;
    fout << "Construction pairs took: " << invert_pair_time << " seconds" << endl;


    vector<vector<int>> queryset = fileReader.coordinates("./data/query_coords_file.txt");
    vector<set<string>> queryprops = fileReader.properties("./data/query_properties_file.txt");
    queryset.resize(1000);
    queryset.shrink_to_fit();
    queryprops.resize(1000);
    queryprops.shrink_to_fit();
    cout << "Query files read..." << endl;

    Utils utility;
    cout << "utility passed" << endl;
    for (int prop = 0; prop < 8; prop++)
    {
        for (int x = 0; x < queryprops.size(); x++)
        {
            set<string> prop_set;
            prop_set.insert(to_string(prop));
            queryprops[x] = prop_set;
        }
        cout << "up to ground truth" << endl;
        vector<vector<int>> answer = utility.computeGroundTruth(queryset, queryprops, data, properties, 100);
        
        cout << "ground truth passed" << endl;
        for (int num_distances = 1000; num_distances <= 10000; num_distances += 1000)
        {
            double filter_time = 0;
            double filter_pair_time = 0;
            double search_time = 0;
            vector<vector<int>> query = prefilter.query(queryset, queryprops, 100, num_distances, filter_time, filter_pair_time, search_time);
            
            cout << "Querying worked for Prop: " << prop << " Distances: " << num_distances << endl;
            
            double accuracy = utility.computeRecall(answer, query);
            cout << "Accuracy of: " << accuracy << endl;
            fout << prop << "\t" << num_distances << "\t" << accuracy << "\t" << filter_time << "\t" << filter_pair_time << "\t" << search_time << endl;
            if (accuracy == 1)
            {
                break;
            }
        }
    }

    // vector<vector<int>> query = prefilter.query(queryset, queryprops, 10, 5000);
    // cout << "Querying worked..." << endl;
    // cout << query.size() << endl;
    // cout << query[1].size() << endl;
    // cout << query[9].size() << endl;
    // cout << query[5].size() << endl;
    // for (int coord : query[4])
    // {
    //     cout << coord << endl;
    // }

    // vector<vector<int>> answer = utility.computeGroundTruth(queryset, queryprops, data, properties, 10);
    // cout << "Proper answer found..." << endl;
    // cout << answer.size() << endl;
    // cout << answer[1].size() << endl;
    // cout << answer[9].size() << endl;
    // for (int coord : answer[4])
    // {
    //     cout << coord << endl;
    // }
    // double accuracy = utility.computeRecall(answer, query);
    // cout << "accuracy:" << accuracy << endl;
}