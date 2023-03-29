#include <iostream>
#include <fstream>
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
#include <sys/stat.h>
#include <chrono>

#include "readfile.h"
#include "utils.h"
#include "FilterIndex.h"
#include <faiss/Clustering.h>
#include <faiss/IndexFlat.h>
#include <bits/stdc++.h>

//include something for map
using namespace std;
using namespace faiss;

// #ifdef _DEBUG
// #define initclock(stuff) std::cout << (stuff);
// #define markclock(stuff) std::cout << (stuff);

// #else
// #define PRINTSTUFF(stuff)
// #endif

template <typename S>
// to print vectors
ostream& operator<<(ostream& os, const vector<S>& vector){
    // Printing vector elements using <<
    for (auto element : vector) {
        os << element << " ";
    }
    return os;
}

FilterIndex::FilterIndex(float* data, size_t d_, size_t nb_, size_t nc_, vector<vector<string>>properties_){
    dataset = data; // data
    d =d_; // dim
    nb = nb_; //num data points
    nc = nc_; // num clusters

    //transform properties to int
    properties.resize(nb);
    uint16_t cnt=0;
    for (int i=0; i<nb; i++){
        for (string prp: properties_[i]){
            if (prLook.count(prp)==0){
                prLook[prp]=cnt;
                cnt++;
            }
            properties[i].push_back(prLook[prp]);
        }
        sort(properties[i].begin(), properties[i].end()); // beware:: sorting the properties will loose the position information!!
    }
    cout<<cnt<<" unique constraints"<<endl;
    // point ids against each cluster ID, flattened
}
//get  ClusterProperties and miniClusters
void FilterIndex::get_kmeans_index(string metric, string indexpath){
    centroids = new float[d*nc]; //provide random nc vectors
    cen_norms = new float[nc]{0};
    data_norms = new float[nb]{0};
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc+1]{0};
    //init: take uniform random points from the cluster
    int v[nb];
    randomShuffle(v , 0, nb);
    Clustering clus(d, nc);
    clus.centroids.resize(d*nc);
    for(uint32_t i = 0; i < nc; ++i) { 
        for(uint32_t j = 0; j < d; ++j){
            clus.centroids[i*d+j] = dataset[v[i]*d +j];
        }
        // memcpy(clus.centroids + i*d, dataset +v[i]*d, sizeof(*centroids) * d);
    }
    clus.verbose = d * nb * nc > (1L << 30);
    // display logs if > 1Gflop per iteration

    IndexFlatL2 index(d);
    clus.train(nb, dataset, index);
    memcpy(centroids, clus.centroids.data(), sizeof(*centroids) * d * nc);
    cout<<"centroids size: "<<clus.centroids.size()<<endl; //centroids (nc * d) if centroids are set on input to train, they will be used as initialization
    
    // if L2 get norms as well
    for(uint32_t j = 0; j < nc; ++j){ 
        cen_norms[j]=0; 
        for(uint32_t k = 0; k < d; ++k) {                 
            cen_norms[j] += centroids[j*d +k]*centroids[j*d +k];        
        } 
        cen_norms[j] = cen_norms[j]/2;
    }
    //centroids (nc * d) if centroids are set on input to train, they will be used as initialization
    for(uint32_t j = 0; j < nb; ++j){  
        for(uint32_t k = 0; k < d; ++k) {    
            data_norms[j]=0;             
            data_norms[j] += dataset[j*d +k]*dataset[j*d +k];        
        } 
        data_norms[j]=data_norms[j]/2;
    }
    //observation: clusters are not balanced if not initiliased with random vectors

    uint32_t* invLookup = new uint32_t[nb];
    // Lookup= new uint32_t[nb];
    // counts = new uint32_t[nc+1]{0};
    // this needs to be integrated in clustering, use openmp, use Strassen algorithm for matmul*
    //get best score cluster
    #pragma omp parallel for  
    for(uint32_t i = 0; i < nb; ++i) {  
        float bin, minscore, temp;
        minscore = 1000000;      
        for(uint32_t j = 0; j < nc; ++j){  
            temp =0;
            for(uint32_t k = 0; k < d; ++k) {                 
                temp += pow(dataset[i*d + k] - centroids[j*d +k], 2);        
            } 
            if (temp<minscore) {
                minscore=temp;
                bin = j;}    
        }
        invLookup[i] = bin;    
    }
    // for(int i = 0; i < 10; ++i) {  
    //     cout<<invLookup[i]<<" ";
    // }
    for(uint32_t i = 0; i < nb; ++i) {
        counts[invLookup[i]+1] = counts[invLookup[i]+1]+1; // 0 5 4 6 3
    }
    for(uint32_t j = 1; j < nc+1; ++j) {
        counts[j] = counts[j]+ counts[j-1]; //cumsum 
    }
    
    //argsort invLookup to get the Lookup
    iota(Lookup, Lookup+nb, 0);
    stable_sort(Lookup, Lookup+nb,
        [&invLookup](size_t i1, size_t i2) {return invLookup[i1] < invLookup[i2];});
    mkdir(indexpath.c_str(), 0777);
    FILE* f1 = fopen((indexpath+"/centroids.bin").c_str(), "w");
    fwrite(centroids, sizeof(float), nc*d, f1);
    FILE* f2 = fopen((indexpath+"/centroidsNorms.bin").c_str(), "w");
    fwrite(cen_norms, sizeof(float), nc, f2);
    FILE* f3 = fopen((indexpath+"/dataNorms.bin").c_str(), "w");
    fwrite(data_norms, sizeof(float), nb, f3);
    FILE* f4 = fopen((indexpath+"/Lookup.bin").c_str(), "w");
    fwrite(Lookup, sizeof(uint32_t), nb, f4);
    FILE* f5 = fopen((indexpath+"/counts.bin").c_str(), "w");
    fwrite(counts, sizeof(uint32_t), nc+1, f5);

    this->invLookup = invLookup;
    cout<<"Got Lookup"<<endl;
    get_cluster_propertiesIndex();
    // rank items in bins by distance
}   

void FilterIndex::get_cluster_propertiesIndex(){
    ClusterProperties.resize(nc);
    for (int clID = 0; clID < nc; clID++){
        //only if atleast one eelment in cluster
        if (counts[clID+1]- counts[clID]==0) continue;
        ClusterProperties[clID] = properties[Lookup[counts[clID]]];
        for (int i =counts[clID]+1; i< counts[clID+1]; i++){
            std::vector<uint16_t> tmp;
            std::set_union(ClusterProperties[clID].begin(), ClusterProperties[clID].end(),  // Input iterators for first range 
                              properties[Lookup[i]].begin(), properties[Lookup[i]].end(), // Input iterators for second range 
                              std::back_inserter(tmp));            // Output iterator
            ClusterProperties[clID] = tmp; // do we need temp?
        }
    }
    // cluster Inv Index
    for (int clID = 0; clID < ClusterProperties.size(); clID++){
        for (uint16_t property : ClusterProperties[clID]){
            inverted_index[property].push_back(clID); //constraint:vectorNum
        }
    }
    // Sort each vector in ascending order. 
    for (auto v : inverted_index) {
        std::sort(v.second.begin(), v.second.end());
    }
}

void FilterIndex::loadIndex(string indexpath){
    centroids = new float[d*nc]; //provide random nc vectors
    cen_norms = new float[nc]{0};
    data_norms = new float[nb]{0};
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc+1]{0};
    FILE* f1 = fopen((indexpath+"/centroids.bin").c_str(), "r");
    fread(centroids, sizeof(float), nc*d, f1);       
    FILE* f2 = fopen((indexpath+"/centroidsNorms.bin").c_str(), "r");
    fread(cen_norms, sizeof(float), nc, f2);
    FILE* f3 = fopen((indexpath+"/dataNorms.bin").c_str(), "r");
    fread(data_norms, sizeof(float), nb, f3);
    FILE* f4 = fopen((indexpath+"/Lookup.bin").c_str(), "r");
    fread(Lookup, sizeof(uint32_t), nb, f4);
    FILE* f5 = fopen((indexpath+"/counts.bin").c_str(), "r");
    fread(counts, sizeof(uint32_t), nc+1, f5);
    // reorder data
    dataset_reordered = new float[nb*d];
    data_norms_reordered = new float[nb];
    for(uint32_t i = 0; i < nb; ++i) {
        copy(dataset+Lookup[i]*d, dataset+(Lookup[i]+1)*d , dataset_reordered+i*d);
        data_norms_reordered[i] = data_norms[Lookup[i]];
    }
    delete dataset;
    get_cluster_propertiesIndex();
}

void FilterIndex::query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int max_num_distances){
    chrono::time_point<chrono::high_resolution_clock> t1, t2, t3, t4, t5, t6;
    neighbor_set = new int32_t[nq*num_results]{-1};
    cout<<"num queries: "<<nq<<endl;
    for (size_t i = 0; i < nq; i++){
        // t1 = chrono::high_resolution_clock::now();
        findNearestNeighbor(queryset+(i*d), queryprops[i], num_results, max_num_distances, i);
        // t2 = chrono::high_resolution_clock::now();
        // cout<<"QT:  "<<chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<" ";
    }
}

// get clusters (match_Cids), start from best and do filter then search, till you get topk
void FilterIndex::findNearestNeighbor(float* query, vector<string> Stprops, int num_results, int max_num_distances, size_t qnum)
{   
    // chrono::time_point<chrono::high_resolution_clock> t1, t2, t3, t4, t5, t6;
    // t1 = chrono::high_resolution_clock::now();
    vector<float> topkDist;
    vector<uint16_t> props;
    for (string stprp: Stprops){
        props.push_back(prLook[stprp]);
    }
    sort(props.begin(), props.end());
    // t2 = chrono::high_resolution_clock::now();
    vector<uint32_t> match_Cids = satisfyingIDs(props); // compare property match and cluster distance computations times. Which is bigger??
    //rank match_Cids
    // t3 = chrono::high_resolution_clock::now();
    priority_queue<pair<float, uint32_t> > pq;
    float sim;
    for (uint32_t id: match_Cids){
        // sim = L2sim(query, centroids+id*d, cen_norms[id], d);
        sim= L2SqrSIMD16ExtAVX(query, centroids+id*d, cen_norms[id], d);
        pq.push({sim, id});//max heap
    }
    // t4 = chrono::high_resolution_clock::now();
    // for each id in pq
    // filter then search bruteforce
    priority_queue<pair<float, uint32_t> > Candidates_pq;
    int seen=0, seenbin=0;
    // need software prefetch or data reordering here
    while(seen<max_num_distances && seenbin<nc){ 
        uint32_t bin = pq.top().second;
        pq.pop();
        seenbin++;
        for (int i =counts[bin]; i< counts[bin+1]; i++){
            //check if constraint statisfies
            //what if an attribute is not present in the corpus- return empty set
            int j =0;
            while (j<props.size() && properties[Lookup[i]][j]== props[j]) j++;
            if (j==props.size()){
                seen++;
                sim = L2sim(query, dataset_reordered +i*d, data_norms_reordered[i], d);
                Candidates_pq.push({sim, Lookup[i]});
            }
            // if (properties[Lookup[i]]== props){ //comparing two vectors fully is slow
            //     seen++;
            //     dist = L2sq(query, dataset +Lookup[i]*d, d);
            //     Candidates_pq.push({-dist, Lookup[i]});
            // }
        }
    }
    // cout<<seen<<endl;
    // t5 = chrono::high_resolution_clock::now();
    for (int i =0; i< min(seen,num_results); i++){ 
        neighbor_set[qnum*num_results+ i] = Candidates_pq.top().second;
        Candidates_pq.pop();
    }
    // t6 = chrono::high_resolution_clock::now();
    // cout<<"1 "<<chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<" ";
    // cout<<"2 "<<chrono::duration_cast<chrono::microseconds>(t3 - t2).count()<<" ";
    // cout<<"3 "<<chrono::duration_cast<chrono::microseconds>(t4 - t3).count()<<" ";
    // cout<<"4 "<<chrono::duration_cast<chrono::microseconds>(t5 - t4).count()<<" ";
    // cout<<"5 "<<chrono::duration_cast<chrono::microseconds>(t6 - t5).count()<<" ";
    // cout<<endl;
}

vector<uint32_t> FilterIndex::satisfyingIDs(vector<uint16_t> props)
{
    // Should take O(set size)
    //use uint16 or uint8 to reduce the size
    vector<uint32_t> match_ids = inverted_index[props[0]];
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
    }
    return match_ids;
}

