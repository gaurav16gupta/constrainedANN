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
#include "readfile.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>

#include <cstdio>
#include "utils.h"
#include <bits/stdc++.h>
//Use quotations for a .h file

//include something for map
using namespace std;

// double computeRecall(vector<vector<int>> answer, vector<vector<int>> guess)
// {
//     //We assume that the size of the answer and guess vector are the same
//     //We assume that the answer has no repeats and is full for each vector
//     double total = 0;
//     double correct = 0;
//     for (size_t x = 0; x < answer.size(); x++)
//     {
//         double subtotal = 0;
//         double subcorrect = 0;
//         set<int> guessSet;
//         for (int guessID : guess[x])
//         {
//             guessSet.insert(guessID);
//         }
//         //Checks how many of the actual closest elements are in the guess set
//         for (int ansID : answer[x])
//         {
//             total++;
//             subtotal++;
//             if (guessSet.count(ansID))
//             {
//                 correct++;
//                 subcorrect++;
//             }
//         }
//         cout << "Trial " << x << " accuracy: " << subcorrect/subtotal << endl;
//     }
//     cout << "total: " << total << endl;
//     cout << "correct: " << correct << endl;
//     double percentage = correct / total;
//     return (percentage);
// }

double RecallAtK(int* answer, int* guess, int k){
    std::vector<int> tmp;
    std::set_intersection(answer, answer+k,  // Input iterators for first range 
                              guess, guess+k, // Input iterators for second range 
                              std::back_inserter(tmp));
    return (double(tmp.size())/double(k));
}


// vector<vector<int>> computeGroundTruth(vector<vector<int>> queryset, vector<set<string>> queryprops, vector<vector<int>> data, vector<set<string>> properties, int num_results)
// {
//     vector<vector<int>> answers;
//     for (int x = 0; x < queryset.size(); x++)
//     {
//         vector<int> answer;
//         set<string> props = queryprops[x];
//         vector<int> validIDs;
//         for (int y = 0; y < properties.size(); y++)
//         {
//             bool valid = true;
//             for (auto property : props)
//             {
//                 if (!properties[y].count(property))
//                 {
//                     valid = false;
//                 }
//             }
//             if (valid)
//             {
//                 validIDs.push_back(y);
//             }
//         }
//         cout << "num validIDs: " << validIDs.size() << endl;

//         vector<vector<int>> validCoords;
//         for (int a = 0; a < validIDs.size(); a++)
//         {
//             validCoords.push_back(data[validIDs[a]]);
//         }

//         cout << "num validCoords: " << validCoords.size() << endl;
        
//         vector<uint32_t> topArgs = argTopK(queryset[x], validCoords, num_results);

//         vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, int k, float* topkDist){


//         cout << "topArgs: ";
//         for (int arg : topArgs)
//         {
//             cout  << arg << ", ";
//         }
//         cout << endl;

//         for (int z = 0; z < topArgs.size(); z++)
//         {
//             answer.push_back(validIDs[topArgs[z]]);
//         }
//         answers.push_back(answer);
//     }
//     return answers;
// }

vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, uint32_t k, vector<float> topkDist){
    float dist; 
    vector<uint32_t> topk;
    priority_queue<pair<float, uint32_t> > pq;
    if (idxSize ==N){
        for (uint32_t i = 0; i < N; i++){
            //L2
            dist =0;
            for (size_t j = 0; j < d; j++){
                dist += pow(vectors[i*d+j] - query[j], 2);
            }
            dist = sqrt(dist);
            //topk
            if (i<k) pq.push({dist, i});
            else{
                if (dist< pq.top().first){
                    pq.pop();
                    pq.push({dist, i});
                }
            }
            //
        }
    }
    else{
        for (uint32_t i = 0; i < idxSize; i++){
            //L2
            try{
            dist =0;
            for (size_t j = 0; j < d; j++){
                dist += pow(vectors[idx[i]*d+j] - query[j], 2);//*
            }
            // cout<<i<<endl;
            // cout<<idx.size()<<endl;
            // cout<<idx[i]<<endl;
            // cout<<endl;

            dist = sqrt(dist);
            //topk
            if (i<k) pq.push({dist, idx[i]});
            else{
                if (dist< pq.top().first){
                    pq.pop();
                    pq.push({dist, idx[i]});
                }
            }
            }
            catch(int mynum){
                
                cout << "Error number: "; 
            }
            //
        }
    }
    for (uint32_t i = 0; i < k; i++){
        topk.push_back(pq.top().second);
        topkDist.push_back(pq.top().first);
        pq.pop();
    }
    return topk;
}

// replace vectorization: avx 256/512
float L2sq(float* a, float* b, size_t d){
    float dist=0;
    for(uint32_t k = 0; k < d; ++k) {                 
        dist += pow(a[k] - b[k],2); 
    } 
    return dist;
}

// vector<int> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, uint32_t* idx, uint32_t idxSize, int num_results)
// {
//     vector<tuple<int, double>> top_k;
//     for (int x = 0; x < num_results; x ++)
//     {
//         top_k.push_back({x, -1.0});
//     }
//     for (int index = 0; index < coords.size(); index++)
//     {
//         double dist = 0;
//         int id = index;
//         for (int coord = 0; coord < query.size(); coord++)
//         {
//             dist += pow(coords[id][coord] - query[coord], 2);
//         }
//         dist = pow(dist, 0.5); 
//         for (int order = 0; order < num_results; order++)
//         {
//             if (dist < get<1>(top_k.at(order)) || get<1>(top_k.at(order)) < 0)
//             {
//                 top_k.insert(top_k.begin() + order, {id, dist});
//                 top_k.pop_back();
//                 break;
//             }
//         }
//     }
//     vector<int> nearest_neighbors;
//     for (int index = 0; index < num_results; index++)
//     {
//         nearest_neighbors.push_back(get<0>(top_k.at(index)));
//     }
//     return nearest_neighbors;
// }

vector<int> argsort(int * query, int length)
{
    //A helper function to find the top num_results args in the coords closest to query
    //O(nk)
    vector<tuple<int, int>> sorted;
    for (int x = 0; x < length; x ++)
    {
        sorted.push_back({x, -1.0});
    }
    for (int index = 0; index < length; index++)
    {
        for (int order = 0; order < length; order++)
        {
            if (query[index] < get<1>(sorted.at(order)) || get<1>(sorted.at(order)) < 0)
            {
                sorted.insert(sorted.begin() + order, {index, query[index]});
                sorted.pop_back();
                break;
            }
        }
    }
    vector<int> nearest_neighbors;
    for (int index = 0; index < length; index++)
    {
        nearest_neighbors.push_back(get<0>(sorted.at(index)));
    }
    return nearest_neighbors;
}

vector<int> vectorArgsort(vector<double> query)
{
    //A helper function to find the top num_results args in the coords closest to query
    //O(nk)
    vector<tuple<int, double>> sorted;
    for (size_t x = 0; x < query.size(); x ++)
    {
        sorted.push_back({x, -1.0});
    }
    for (size_t index = 0; index < query.size(); index++)
    {
        for (size_t order = 0; order < query.size(); order++)
        {
            if (query[index] < get<1>(sorted.at(order)) || get<1>(sorted.at(order)) < 0)
            {
                sorted.insert(sorted.begin() + order, {index, query[index]});
                sorted.pop_back();
                break;
            }
        }
    }
    vector<int> nearest_neighbors;
    for (size_t index = 0; index < query.size(); index++)
    {
        nearest_neighbors.push_back(get<0>(sorted.at(index)));
    }
    return nearest_neighbors;
}


// std::vector<int> intersectVectors(const std::vector<std::vector<int>>& vectors) {
//     std::vector<int> result;

//     if (vectors.empty()) {
//         return result;
//     }
//     // Use the first vector as the base for the intersection. 
//     result = vectors[0];

//     // Iterate through the remaining vectors and perform an intersection with the current result. 
//     for (size_t i = 1; i < vectors.size(); ++i) {
//         std::vector<int> tmp;

//         std::set_intersection(result.begin(), result.end(),  // Input iterators for first range 
//                               vectors[i].begin(), vectors[i].end(), // Input iterators for second range 
//                               std::back_inserter(tmp));            // Output iterator

//         result = tmp;   // Update the current result with the intersection of this iteration. 

//         if (result.empty()) {   // If there is no intersection, we can stop here as any further intersections will be empty too. 
//             break;  
//         }    										   // Otherwise, continue to next iteration and intersect with current result again. 
//     }

//     return result;   // Return the final intersection of all input vectors. 
// }

void randomShuffle(int* v , int l, int u){
     // Range of numbers [l, u]
    iota(v, v+u-l, l); 
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v, v+u-l, g);
}