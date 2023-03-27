
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


using namespace std;

double RecallAtK(int* answer, int* guess, size_t k, size_t nq){
    uint32_t count = 0;
    for (int i=0;i<nq;i++){
        sort(answer+ k*i, answer + (i+1)*k);
        sort(guess+ k*i, guess+ (i+1)*k);
        // for (int j=0; j<k; j++){
        //     cout << answer[j+ k*i] << " ";
        // }
        // cout<<endl;
        // for (int j=0; j<k; j++){
        //     cout << guess[j+ k*i] << " ";
        // }
        // cout<<endl;
        std::vector<int> tmp;
        std::set_intersection(answer+ k*i, answer + (i+1)*k,  // Input iterators for first range 
                            guess+ k*i, guess+ (i+1)*k, // Input iterators for second range 
                            std::back_inserter(tmp));
        count += double(tmp.size());
        // cout<<double(tmp.size())<<endl;
        // break;
    }
    return (count/double(nq*k));
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

float L2sim(float* a, float* b, float norm_bsq, size_t d){
    float dist=0;
    for(uint32_t k = 0; k < d; ++k) {    
        dist += a[k]*b[k]; // one unit FLOP- mul
        // dist += pow(a[k]-b[k],2); // two units FLOPS- mul and sub
    } 
    dist= dist- norm_bsq;
    return dist;
}

// Not giving speedup!! Check the issue
float L2SqrSIMD16ExtAVX(float *pVect1, float *pVect2, float norm_bsq, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];

    // __m256 sum256 = _mm256_set1_ps(0);
    // for(uint32_t k = 0; k < 32; ++k) {   
    //     __m256 dot1 = _mm256_mul_ps(_mm256_loadu_ps(pVect1), _mm256_loadu_ps(pVect2));
    //     pVect1 += 8;
    //     pVect2 += 8;
    //     sum256 = _mm256_add_ps(dot1, sum256); 
    // }

    size_t qty16 = qty / 16;
    const float *pEnd1 = pVect1 + 16 * qty16;
    __m256 sum256 = _mm256_set1_ps(0);
    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
        const __m256 v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        const __m256 v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum256 = _mm256_add_ps(sum256, _mm256_mul_ps(v1, v2));
    }
    _mm256_store_ps(TmpRes, sum256);
    float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] - norm_bsq;
    return sum;
}

uint16_t getclusterPart(uint16_t* maxMC, vector<uint16_t> &props, int treelen){
    // maxMC: attribute number, property, frequency
    for (uint16_t i=0;i<treelen; i++){
        if (maxMC[i*3+1] == props[maxMC[i*3+0]]){
            return i;
        }
    }
    return treelen+1;    
}

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