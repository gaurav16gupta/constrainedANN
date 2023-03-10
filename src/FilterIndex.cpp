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
#include <chrono>

#include "cbloomfilter.h"
#include "crambo.h"
#include "readfile.h"
#include "utils.h"
#include "FilterIndex.h"

#include <faiss/Clustering.h>
#include <faiss/IndexFlat.h>
#include <bits/stdc++.h>

//include something for map
using namespace std;
using namespace faiss;

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
        sort(properties[i].begin(), properties[i].end());
    }
    cout<<cnt<<" unique constrints"<<endl;
    // point ids against each cluster ID, flattened
}
//get  ClusterProperties and miniClusters
void FilterIndex::get_kmeans_index(string metric){
    // float a = kmeans_clustering(d, nb, nc, dataset, centroids); 
    centroids = new float[d*nc]; //provide random nc vectors
    cen_norms = new float[nc]{0};
    data_norms = new float[nb]{0};
    bool load = 1; //use load =0 for first time run
    if (load){
        FILE* f1 = fopen("centroids.bin", "r");
        fread(centroids, sizeof(float), nc*d, f1);       
        FILE* f2 = fopen("centroidsNorms.bin", "r");
        fread(cen_norms, sizeof(float), nc, f2);
        FILE* f3 = fopen("dataNorms.bin", "r");
        fread(data_norms, sizeof(float), nb, f3);
    }
    else{
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
        cout<<"num centroids: "<<clus.centroids.size()<<endl; //centroids (nc * d) if centroids are set on input to train, they will be used as initialization
        
        // if L2 get norms as well
        for(uint32_t j = 0; j < nc; ++j){  
            for(uint32_t k = 0; k < d; ++k) {                 
                cen_norms[j] += centroids[j*d +k]*centroids[j*d +k];        
            } 
        }
        //centroids (nc * d) if centroids are set on input to train, they will be used as initialization
        for(uint32_t j = 0; j < nb; ++j){  
            for(uint32_t k = 0; k < d; ++k) {                 
                data_norms[j] += dataset[j*d +k]*dataset[j*d +k];        
            } 
        }
        FILE* f1 = fopen("centroids.bin", "w");
        fwrite(centroids, sizeof(float), nc*d, f1);
        FILE* f2 = fopen("centroidsNorms.bin", "w");
        fwrite(cen_norms, sizeof(float), nc, f2);
        FILE* f3 = fopen("dataNorms.bin", "w");
        fwrite(data_norms, sizeof(float), nb, f3);
        //observation: clusters are not balanced
    }

    uint32_t* invLookup = new uint32_t[nb];
    Lookup= new uint32_t[nb];
    counts = new uint32_t[nc+1]{0};
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
    for(uint32_t j = 1; j < nc; ++j) {
        counts[j] = counts[j]+ counts[j-1]; //cumsum 
      }
    
    //argsort invLookup to get the Lookup
    iota(Lookup, Lookup+nb, 0);
    stable_sort(Lookup, Lookup+nb,
        [&invLookup](size_t i1, size_t i2) {return invLookup[i1] < invLookup[i2];});

    this->invLookup = invLookup;
    cout<<"Got Lookup"<<endl;
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

void FilterIndex::query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int max_num_distances){
    // vector<vector<uint32_t>> neighbor_set;
    // string s;
    // cin>>s;
    neighbor_set = new int32_t[nq*num_results];
    cout<<"num queries: "<<nq<<endl;
    for (size_t i = 0; i < nq; i++){
        findNearestNeighbor(queryset+(i*d), queryprops[i], num_results, max_num_distances, i);
    }
}

// float FilterIndex::PartialL2(float* a, float* b, float* b_norm, uint32_t id, float dist){
//     dist=0;
//     for(uint32_t k = 0; k < d; ++k) {                 
//         dist += a[k] * b[id*d +k];        
//     } 
//     dist = b_norm[id]- 2*dist;
//     return dist;
// }

float FilterIndex::L2(float* a, float* b, uint32_t id){
    float dist=0;
    for(uint32_t k = 0; k < d; ++k) {                 
        dist += pow(a[k] - b[id*d +k],2); 
    } 
    return dist;
}

// get clusters (match_Cids), start from best and do filter then search, till you get topk
void FilterIndex::findNearestNeighbor(float* query, vector<string> Stprops, int num_results, int max_num_distances, size_t qnum)
{   
    vector<float> topkDist;
    vector<uint16_t> props;
    for (string stprp: Stprops){
        props.push_back(prLook[stprp]);
    }
    sort(props.begin(), props.end());
    vector<uint32_t> match_Cids = satisfyingIDs(props); // compare property match and cluster distance computations times. Which is bigger??
    //rank match_Cids
    priority_queue<pair<float, uint32_t> > pq;
    float dist;
    for (uint32_t id: match_Cids){
        dist = L2sq(query, centroids+id*d, d);
        // dist = L2(query, centroids, id);
        pq.push({-dist, id});
    }
    // for each id in pq
    // filter then search bruteforce
    priority_queue<pair<float, uint32_t> > Candidates_pq;
    int seen=0, seenbin=0;
    while(seen<max_num_distances){
        uint32_t bin = pq.top().second;
        pq.pop();
        seenbin++;
        for (int i =counts[bin]; i< counts[bin+1]; i++){
            //check if constraint statisfies
            //what if an attribute is not present in the corpus
            
            if (properties[Lookup[i]]== props){ //comparing two string vectors
                seen++;
                dist = L2(query, dataset, Lookup[i]);
                Candidates_pq.push({-dist, Lookup[i]});
            }
        }
    }
    for (int i =0; i< num_results; i++){ 
        neighbor_set[qnum*num_results+ i] = Candidates_pq.top().second;
        Candidates_pq.pop();
        // cnt++;
    }
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



int main()
{
    cout << "filterIndex running..." << endl;
    size_t d, nb,nc, nq, num_results; 
    // float* xt = fvecs_read("../data/sift/sift/sift_learn.fvecs", &d, &nt); // not needed now
    float* data = fvecs_read("data/sift/sift_base.fvecs", &d, &nb);
    vector<vector<string>> properties = getproperties("data/sift/label_sift_base.txt",' ');
    cout<<properties.size()<<endl;
    cout << "Data files read" << endl;
    nc = 1000; // num clusters
    FilterIndex myFilterIndex(data, d, nb, nc, properties);
    string metric="L2";

    myFilterIndex.get_kmeans_index(metric);
    myFilterIndex.get_cluster_propertiesIndex();
    // for (int i; i<10; i++){
    //     cout<<myFilterIndex.ClusterProperties[i]<<endl;
    // }
    cout << "Indexed" << endl;

    float* queryset = fvecs_read("data/sift/sift_query.fvecs", &d, &nq);
    vector<vector<string>> queryprops = getproperties("data/sift/label_sift_query.txt",' ');
    int* queryGTlabel = ivecs_read("data/sift/label_sift_hard_groundtruth.ivecs", &num_results, &nq);
    cout << "Query files read..." << endl;

    nq = 1000;  //overwrite 
    chrono::time_point<chrono::high_resolution_clock> t1, t2, t3;
    t1 = chrono::high_resolution_clock::now();
    myFilterIndex.query(queryset, nq, queryprops, num_results, 2500);
    t2 = chrono::high_resolution_clock::now();
    cout << "query per sec "<<(double)1000000*nq/(double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count()<<endl;;
    int32_t* output = myFilterIndex.neighbor_set;
    int output_[num_results*nq];
    copy(output, output+num_results*nq , output_);
    
    // int* queryGT = ivecs_read("../data/sift/sift/sift_groundtruth.ivecs", &k, &nq);

    //recall
    double recall = RecallAtK(queryGTlabel, output_, num_results, nq);
    cout<<"Recall10@10: "<<recall<<endl;
}

//*https://github.com/vaibhawvipul/performance-engineering/blob/main/matrix_multiplication/matrix_strassen_looporder_omp.cpp