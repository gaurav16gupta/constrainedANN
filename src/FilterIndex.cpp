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
#include "FilterIndex.h"

#include <faiss/Clustering.h>
#include <faiss/IndexFlat.h>
#include <bits/stdc++.h>

//include something for map
using namespace std;
using namespace faiss;

template <typename S>
ostream& operator<<(ostream& os, const vector<S>& vector){
    // Printing vector elements using <<
    for (auto element : vector) {
        os << element << " ";
    }
    return os;
}

FilterIndex::FilterIndex(float* data, size_t d_, size_t nb_, size_t nc_, size_t k_, vector<vector<string>>properties_){
    dataset = data; // data
    d =d_; // dim
    nb = nb_; //num data points
    k = k_; // topk op
    nc = nc_; // num clusters

    //allocate dynamically if stack overflows
    // float centroids[d*nc]; 
    properties = properties_;
    // point ids against each cluster ID, flattened
}
//get  ClusterProperties and miniClusters
void FilterIndex::get_kmeans_index(string metric){
    // float a = kmeans_clustering(d, nb, nc, dataset, centroids); 
    centroids = new float[d*nc]; //provide random nc vectors
    cen_norms = new float[nc]{0};
    data_norms = new float[nb]{0};
    bool load = 1;
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
        // cout<<centroids[0]<<" "<<dataset[v[0]*d]<<endl;
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
                temp += dataset[i*d + k] * centroids[j*d +k];        
            } 
            if ((cen_norms[i]- 2*temp)<minscore) {
                minscore=temp;
                bin = j;}    
        }
        invLookup[i] = bin;    
    }
    // for(int i = 0; i < 10; ++i) {  
    //     cout<<invLookup[i]<<" ";
    // }
    cout<<"Got invLookup"<<endl;
    for(uint32_t i = 0; i < nb; ++i) {
        counts[invLookup[i]+1] = counts[invLookup[i]+1]+1;
      }
    for(uint32_t j = 1; j < nc; ++j) {
        counts[j] = counts[j]+ counts[j-1]; //cumsum
      }
    
    //argsort invLookup to get the Lookup
    iota(Lookup, Lookup+nb, 0);
    stable_sort(Lookup, Lookup+nb,
        [&invLookup](size_t i1, size_t i2) {return invLookup[i1] < invLookup[i2];});
    for(int i = 0; i < 10; ++i) {  
        cout<<counts[i]<<" ";
    }
    // for(int i = 0; i < 10; ++i) {  
    //     cout<<Lookup[i]<<" ";
    // }

    this->invLookup = invLookup;
    // this->Lookup = Lookup;
    // this->counts = counts;
    cout<<"Got Lookup"<<endl;
}   

void FilterIndex::get_cluster_propertiesIndex(){
    ClusterProperties.resize(nc);
    for (int clID = 0; clID < nc; clID++){
        //only if atleast one eelment in cluster
        if (counts[clID+1]- counts[clID]==0) continue;
        ClusterProperties[clID] = properties[Lookup[counts[clID]]];
        for (int i =counts[clID]+1; i< counts[clID+1]; i++){
            std::vector<string> tmp;
            
            std::set_union(ClusterProperties[clID].begin(), ClusterProperties[clID].end(),  // Input iterators for first range 
                              properties[Lookup[i]].begin(), properties[Lookup[i]].end(), // Input iterators for second range 
                              std::back_inserter(tmp));            // Output iterator
            ClusterProperties[clID] = tmp; // do we need temp?
        }
    }
    
    // cluster Inv Index
    for (int clID = 0; clID < ClusterProperties.size(); clID++){
        for (string property : ClusterProperties[clID]){
            inverted_index[property].push_back(clID); //constraint:vectorNum
        }
    }
    // Sort each vector in ascending order. 
    for (auto v : inverted_index) {
        std::sort(v.second.begin(), v.second.end());
    }
}

vector<vector<uint32_t>> FilterIndex::query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int max_num_distances, int m){
    vector<vector<uint32_t>> neighbor_set;
    // string s;
    // cin>>s;
    cout<<nq<<endl;
    for (size_t i = 0; i < nq; i++){
        neighbor_set.push_back(findNearestNeighbor(queryset+(i*d), queryprops[i], num_results, max_num_distances, m));
    }
    return neighbor_set;
}

float FilterIndex::PartialL2(float* a, float* b, float* b_norm, uint32_t id, float dist){
    dist=0;
    for(uint32_t k = 0; k < d; ++k) {                 
        dist += a[k] * b[id*d +k];        
    } 
    dist = b_norm[id]- 2*dist;
    return dist;
}

// get clusters (match_Cids), start from best and do filter then search, till you get topk
vector<uint32_t> FilterIndex::findNearestNeighbor(float* query, vector<string> props, int num_results, int max_num_distances, int m)
{
    vector<float> topkDist;
    vector<uint32_t> match_Cids = satisfyingIDs(props); // compare property match and cluster distance computations times. Which is bigger??
    //rank match_Cids
    priority_queue<pair<float, uint32_t> > pq;
    float dist;
    for (uint32_t id: match_Cids){
        cout<<"id: "<< id<<endl;
        PartialL2(query, centroids, cen_norms, id, dist);
        pq.push({dist, id});
    }
    
    // for each id in pq
    // filter then search bruteforce
    priority_queue<pair<float, uint32_t> > Candidates_pq;
    uint32_t seen=0, seenbin=0;
    while(seen<k && seenbin<m){
        uint32_t bin = pq.top().second;
        pq.pop();
        seenbin++;
        for (int i =counts[bin]; i< counts[bin+1]; i++){
            //check if constraint statisfies
            if (properties[Lookup[i]]== props){ //comparing two string vectors
                seen++;
                PartialL2(query, dataset, data_norms, Lookup[i], dist);
                Candidates_pq.push({dist, Lookup[i]});
            }
        }
    }
    vector<uint32_t> topk;
    topk.resize(k);
    for (int i =1; i< k; i++){ 
        topk[i] = Candidates_pq.top().second;
        Candidates_pq.pop();
    } 
    return topk;
}

vector<uint32_t> FilterIndex::satisfyingIDs(vector<string> props)
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

    size_t d=128, nb=1000000,nc=1000, nq; 
    // float* xt = fvecs_read("../data/sift/sift/sift_learn.fvecs", &d, &nt); // not needed now
    float* data = fvecs_read("../data/sift/sift/sift_base.fvecs", &d, &nb);
    // for (int j=0;j<10;j++){
    //     for (int i=0;i<128;i++){
    //         cout<<data[i+j*128]<<' ';
    //     }
    //     cout<<endl;
    // }
    vector<vector<string>> properties = getproperties("../data/sift/sift_label/label_sift_base.txt",' ');
    cout<<properties.size()<<endl;
    cout << "Data files read" << endl;
    FilterIndex myFilterIndex(data, d, nb, nc, 10, properties);
    string metric="L2";

    myFilterIndex.get_kmeans_index(metric);
    myFilterIndex.get_cluster_propertiesIndex();
    // for (int i; i<10; i++){
    //     cout<<myFilterIndex.ClusterProperties[i]<<endl;
    // }
    cout << "Indexed" << endl;

    float* queryset = fvecs_read("../data/sift/sift/sift_query.fvecs", &d, &nq);
    vector<vector<string>> queryprops = getproperties("../data/sift/sift_label/label_sift_query.txt",' ');
    cout << "Query files read..." << endl;
    nq = 100;
    vector<vector<uint32_t>> query = myFilterIndex.query(queryset, nq, queryprops, 10, 300, 20);
    cout << "Querying done" << endl;
    for (int coord : query[0]){
        cout << coord << " ";
    }
    // cout<<endl;
    // //load groundtruth
    // int* queryGT = ivecs_read("../data/sift/sift/label_sift_hard_groundtruth.ivecs", &d2, &nq);
    // for (int i; i<10; i++){
    //     cout<<queryGT[i]<<" ";
    // }

}

//*https://github.com/vaibhawvipul/performance-engineering/blob/main/matrix_multiplication/matrix_strassen_looporder_omp.cpp