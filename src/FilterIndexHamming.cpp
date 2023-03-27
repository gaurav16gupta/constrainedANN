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
    treelen = 4; //length of hamming tree, num miniclusters= treelen+1

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
        sort(properties[i].begin(), properties[i].end());
    }
    cout<<cnt<<" unique constraints"<<endl; 
    // Properties to attribute location map
    int numAttr = properties[0].size();
    for (int i=0; i<nb; i++){
        for (int j=0; j<numAttr; j++){
            PrpAtrMap[properties[i][j]] = j;
        }
    }
}

//NN index
// TODO: have more partitioning methods. Include BLISS
void FilterIndex::get_kmeans_index(string metric, string indexpath){
    centroids = new float[d*nc]; //provide random nc vectors
    cen_norms = new float[nc]{0};
    data_norms = new float[nb]{0};
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc+1]{0};
    //init: Take uniform random points from the corpus. They will be used as kmeans initialization
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
    cout<<"centroids size: "<<clus.centroids.size()<<endl; //centroids (nc * d)
    
    // if L2 get norms as well
    for(uint32_t j = 0; j < nc; ++j){ 
        cen_norms[j]=0; 
        for(uint32_t k = 0; k < d; ++k) {                 
            cen_norms[j] += centroids[j*d +k]*centroids[j*d +k];        
        } 
        cen_norms[j] = cen_norms[j]/2;
    }
    for(uint32_t j = 0; j < nb; ++j){  
        for(uint32_t k = 0; k < d; ++k) {    
            data_norms[j]=0;             
            data_norms[j] += dataset[j*d +k]*dataset[j*d +k];        
        } 
        data_norms[j]=data_norms[j]/2;
    }
    //observation: clusters are not balanced if not initiliased with random vectors from the corpus
    uint32_t* invLookup = new uint32_t[nb];
    // this needs to be integrated in clustering, use openmp, use Strassen algorithm for matmul*
    //get best score cluster
    #pragma omp parallel for  
    for(uint32_t i = 0; i < nb; ++i) {  
        float bin, minscore, temp;// per thread local variables
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
//checks if the property x is seen before in maxMC
bool FilterIndex::not_in(uint16_t x, uint16_t* a, int h){
    // attribute number, property, frequency
    if (h == 0){
        return 1;
    } 
    else{
        for(uint16_t i=0;i< h;i++){ 
            if (a[i*3+1]==x){return 0;}
        };
        return 1;
    }
}

//get most seen constraint
// exclude and get most seen next constraint
void FilterIndex::get_mc_propertiesIndex(){
    vector<vector<uint32_t>> maxMCIDs; //nested array to store the mini-clusters
    maxMCIDs.resize((treelen+1)*nc);
    maxMC = new uint16_t[(treelen+1)*nc]; 
    counts[nc]=1000000; // To remove this later
    for (int clID = 0; clID < nc; clID++){
        if (counts[clID+1]- counts[clID]==0) continue;
        // get count of vector properties        
        //get the max
        for (int h=0; h<treelen; h++){
            unordered_map<uint16_t, int> freq;
            for (int i =counts[clID]; i< counts[clID+1]; i++){
                for (uint16_t x: properties[Lookup[i]]) {
                    if(not_in(x, maxMC + (treelen+1)*clID, h )) freq[x]++;
                }
            }
            //choose property with max count
            auto maxElement = max_element(freq.begin(), freq.end(),
                [](const pair<uint16_t, int>& p1, const pair<uint16_t, int>& p2) { return p1.second < p2.second;}
            );
            maxMC[(treelen+1)*clID+h+0]= PrpAtrMap[maxElement->first];
            maxMC[(treelen+1)*clID+h+1]= maxElement->first;
            maxMC[(treelen+1)*clID+h+2]= maxElement->second;
            // maxMC.push_back(make_pair(PrpAtrMap[maxElement->first], *maxElement));
        }
        
        //maxMC serves as a node list and node data size in hamming tree, where
        //node: property from maxMC
        //node data: corresponding vector IDs
        for (int i =counts[clID]; i< counts[clID+1]; i++){
            for (int j=0; j< treelen; j++){
                // if(properties[Lookup[i]][maxMC[j].first]==maxMC[j].second.first){
                if(properties[Lookup[i]][maxMC[(treelen+1)*clID+j]]==maxMC[(treelen+1)*clID+j+1]){ 
                    maxMCIDs[j + (treelen+1)*clID].push_back(Lookup[i]);
                    goto m_label;
                }
            }
            maxMCIDs[treelen+ (treelen+1)*clID].push_back(Lookup[i]);
            m_label:;
        } 
    }
    //update Lookup, counts. Flatten the maxMCIDs into Lookup
    //each cluster now spans treelen+1 buckets
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc*(treelen+1)+1]{0}; 
    for (int clID = 0; clID < nc; clID++){
        for (int j=0; j< treelen+1; j++){
            int id = clID*(treelen+1) +j;
            counts[id+1] = counts[id]+ maxMCIDs[id].size();
            memcpy(Lookup+ counts[id], maxMCIDs[id].data(), sizeof(*Lookup) * maxMCIDs[id].size());
        }
    }
}
//      check for logical errors
//      hamming tree partitons maynot be balanced. Can we balance them?
void FilterIndex::get_cluster_propertiesIndex(){
    ClusterProperties.resize(nc);
    for (int clID = 0; clID < nc; clID++){
        //only if atleast one element in cluster
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
    properties_reordered.resize(nb);
    for(uint32_t i = 0; i < nb; ++i) {
        copy(dataset+Lookup[i]*d, dataset+(Lookup[i]+1)*d , dataset_reordered+i*d);
        data_norms_reordered[i] = data_norms[Lookup[i]];
        properties_reordered[i] = properties[Lookup[i]];
    }
    delete dataset;
    // get_cluster_propertiesIndex();
    get_mc_propertiesIndex();
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

// start from best custer and do filter then search, till you get topk
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

    priority_queue<pair<float, uint32_t> > pq;
    float sim;
    for (uint32_t id=0; id<nc; id++){
        sim = L2SqrSIMD16ExtAVX(query, centroids+id*d, cen_norms[id], d);
        pq.push({sim, id});//max heap
    }
    // for each id in pq
    // filter then search bruteforce
    priority_queue<pair<float, uint32_t> > Candidates_pq;
    int seen=0, seenbin=0;
    // need software prefetch or data reordering here
    while(seen<max_num_distances && seenbin<nc){ 
        uint32_t bin = pq.top().second;
        pq.pop();
        seenbin++;
        bin = bin*(treelen+1);
        //get which disjoint part of the cluster query belongs to
        uint16_t membership = getclusterPart(maxMC+ bin*3 , props, treelen);
        bin = bin+membership;

        for (int i =counts[bin]; i< counts[bin+1]; i++){
            //check if constraint statisfies
            int j =0;
            while (j<props.size() && properties_reordered[i][j]== props[j]) j++;
            if (j==props.size()){
                seen++;
                sim = L2SqrSIMD16ExtAVX(query, dataset_reordered +i*d, data_norms_reordered[i], d);
                Candidates_pq.push({sim, Lookup[i]});
            }
        }
    }
    for (int i =0; i< min(seen,num_results); i++){ 
        neighbor_set[qnum*num_results+ i] = Candidates_pq.top().second;
        Candidates_pq.pop();
    }
    // t2 = chrono::high_resolution_clock::now();
    // cout<<"time: "<<chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<" ";
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
