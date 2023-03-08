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

#include "cbloomfilter.h"
#include "crambo.h"
#include "readfile.h"
#include "utils.h"
#include "searchFilter.h"

#include <faiss/Clustering.h>
#include <faiss/IndexFlat.h>
#include <faiss/index_factory.h>
#include <faiss/AutoTune.h>
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

searchFilter::searchFilter(float* data, size_t d_, size_t nb_, size_t nc_, size_t k_, vector<vector<string>>properties_){
    dataset = data; // data
    d =d_; // dim
    nb = nb_; //num data points
    k = k_; // topk op
    nc = nc_; // num clusters
    IndexFlatL2 index(d);
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
void searchFilter::get_kmeans_index(string metric){
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

        clus.train(nb, dataset, index);
        // index.add(nc, clus.centroids.data());
        // index.add(nb, dataset);
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
    
    //get best score cluster
    // #pragma omp parallel for 
    // for(uint32_t i = 0; i < nb; ++i) { 
    //     vector<uint32_t> idx;
    //     vector<float> topkDist;
    //     vector<uint32_t> topk;
    //     topk= argTopK(dataset+ i*d, centroids, d, nc, idx, nc, 1, topkDist);
    //     invLookup[i] = topk[0];  
    // }
    // // 423462 423569 38326 443868 444875 445443 468794 37651 444475 905274 

    // this needs to be integrated in clustering, use openmp, use Strassen algorithm for matmul*
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
        // 423462 423569 38326 443868 444875 445443 468794 37651 444475 905274 
    }
    for(int i = 0; i < 10; ++i) {  
        cout<<invLookup[i]<<" ";
    }
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
    // for(int i = 0; i < 1000; ++i) {  
    //     cout<<counts[i]<<" ";
    // }
    // for(int i = 0; i < 10; ++i) {  
    //     cout<<Lookup[i]<<" ";
    // }

    this->invLookup = invLookup;
    // this->Lookup = Lookup;
    // this->counts = counts;
    cout<<"Got Lookup"<<endl;
}   

// void searchFilter::get_cluster_propertiesIndex(){
//     ClusterProperties.resize(nc);
//     for (int clID = 0; clID < nc; clID++){
//         //only if atleast one eelment in cluster
//         if (counts[clID+1]- counts[clID]==0) continue;
//         ClusterProperties[clID] = properties[Lookup[counts[clID]]];
//         for (int i =counts[clID]+1; i< counts[clID+1]; i++){
//             std::vector<uint16_t> tmp;
//             std::set_union(ClusterProperties[clID].begin(), ClusterProperties[clID].end(),  // Input iterators for first range 
//                               properties[Lookup[i]].begin(), properties[Lookup[i]].end(), // Input iterators for second range 
//                               std::back_inserter(tmp));            // Output iterator
//             ClusterProperties[clID] = tmp; // do we need temp?
//         }
        
//     }
    
//     // cluster Inv Index
//     for (int clID = 0; clID < ClusterProperties.size(); clID++){
//         for (uint16_t property : ClusterProperties[clID]){
//             inverted_index[property].push_back(clID); //constraint:vectorNum
//         }
//     }
//     // Sort each vector in ascending order. 
//     for (auto v : inverted_index) {
//         std::sort(v.second.begin(), v.second.end());
//     }
// }

void searchFilter::query(float* queryset, int nq, vector<vector<string>> queryprops, int num_results, int max_num_distances, int m){
    // vector<vector<uint32_t>> neighbor_set;
    // string s;
    // cin>>s;

    // by index
    // output buffers
    // idx_t* I = new idx_t[nq * num_results]; //type part of faiss namespace
    // float* D = new float[nq * num_results];
    // index.search(nq, queryset, num_results, D, I);

    neighbor_set = new int32_t[nq*num_results];
    neighbor_dist = new float[nq*num_results];
    cout<<"num queries: "<<nq<<endl;
    for (size_t i = 0; i < nq; i++){
        findNearestNeighbor(queryset+(i*d), queryprops[i], num_results, max_num_distances, m, i);
    }
    // for (int i=0;i<100; i++){
    //     cout<<neighbor_set[i]<<" ";
    //     cout<<(int32_t)I[i]<<endl;
    // }
}

// float FilterIndex::PartialL2(float* a, float* b, float* b_norm, uint32_t id, float dist){
//     dist=0;
//     for(uint32_t k = 0; k < d; ++k) {                 
//         dist += a[k] * b[id*d +k];        
//     } 
//     dist = b_norm[id]- 2*dist;
//     return dist;
// }

float searchFilter::L2(float* a, float* b, uint32_t id){
    float dist=0;
    for(uint32_t k = 0; k < d; ++k) {                 
        dist += pow(a[k] - b[id*d +k],2); 
    } 
    return dist;
}

// get clusters (match_Cids), start from best and do filter then search, till you get topk
void searchFilter::findNearestNeighbor(float* query, vector<string> Stprops, int num_results, int max_num_distances, int m, size_t qnum)
{
    vector<float> topkDist;
    vector<uint16_t> props;
    for (string stprp: Stprops){
        props.push_back(prLook[stprp]); // changes to int attributes
    }
    priority_queue<pair<float, uint32_t> > pq;
    float dist;
    for(uint32_t j = 0; j < nc; ++j) {
        dist = L2(query, centroids, j);
        pq.push({-dist, j});
      }

    priority_queue<pair<float, uint32_t> > Candidates_pq;
    int seen=0, seenbin=0;
    while(seenbin<m){
        uint32_t bin = pq.top().second;
        pq.pop();
        seenbin++;
        for (int i =counts[bin]; i< counts[bin+1]; i++){
            seen++;
            dist = L2(query, dataset, Lookup[i]);
            Candidates_pq.push({-dist, Lookup[i]});
        }
    }
    vector<uint32_t> topk;
    vector<float> topkdist;
    size_t setsize = min(max_num_distances, seen);
    topk.resize(setsize);
    topkdist.resize(setsize);
    // try argpartition
    // if (Candidates_pq.size()>=k){
    for (int i =0; i< setsize; i++){ 
        // cout<<Candidates_pq.top().second<<endl;
        topk[i] = Candidates_pq.top().second;
        topkdist[i] = Candidates_pq.top().first;
        Candidates_pq.pop();
        // cout<<topk[i]<<" ";
    }
    // filter
   
    int cnt= 0;
    for (int i =0; i< setsize; i++){ 
        if (properties[topk[i]]== props && cnt<num_results){
        // if ( cnt<num_results){
            // filtered_topk.push_back(topk[i]);
            neighbor_set[qnum*num_results+ cnt] = topk[i];
            neighbor_dist[qnum*num_results+ cnt] = sqrt(-topkdist[i]);
            cnt++;
        }
    }
    while(cnt<num_results){
        neighbor_set[qnum*num_results+ cnt] = -1;
        neighbor_dist[qnum*num_results+ cnt] = 0;
        cnt++;
    }
}

// vector<uint32_t> searchFilter::satisfyingIDs(vector<uint16_t> props)
// {
//     // Should take O(set size)
//     //use uint16 or uint8 to reduce the size
//     vector<uint32_t> match_ids = inverted_index[props[0]];
//     for (int i =1; i< props.size(); i++){ //loops over all properties
//         // vector<uint32_t> valid_ids = ;
//         std::vector<uint32_t> tmp;
//         std::set_intersection(match_ids.begin(), match_ids.end(),  // Input iterators for first range 
//                               inverted_index[props[i]].begin(), inverted_index[props[i]].end(), // Input iterators for second range 
//                               std::back_inserter(tmp));            // Output iterator

//         match_ids = tmp;   // Update the current result with the intersection of this iteration. 
//         if (match_ids.empty()) {   // If there is no intersection, we can stop here as any further intersections will be empty too. 
//             break;  
//         }    					
//     }
//     return match_ids;
// }



int main()
{
    cout << "filterIndex running..." << endl;

    size_t d=128, nb=1000000,nc=1000, nq, k; 
    float* data = fvecs_read("../data/sift/sift/sift_base.fvecs", &d, &nb);
 
    vector<vector<string>> properties = getproperties("../data/sift/sift_label/label_sift_base.txt",' ');
    cout<<properties.size()<<endl;
    cout << "Data files read" << endl;
    searchFilter myIndex(data, d, nb, nc, 10, properties);
    string metric="L2";

    myIndex.get_kmeans_index(metric);
    // myFilterIndex.get_cluster_propertiesIndex();
    // for (int i; i<10; i++){
    //     cout<<myFilterIndex.ClusterProperties[i]<<endl;
    // }
    cout << "Indexed" << endl;

    float* queryset = fvecs_read("../data/sift/sift/sift_query.fvecs", &d, &nq);
    vector<vector<string>> queryprops = getproperties("../data/sift/sift_label/label_sift_query.txt",' ');
    cout << "Query files read..." << endl;
    nq = 10;
    k = 10;
    myIndex.query(queryset, nq, queryprops, k, 100, 5);
    int32_t* output = myIndex.neighbor_set;
    float* outputdist = myIndex.neighbor_dist;
    cout << "Querying done" << endl;
    for (int i=0; i<k; i++){
        cout << output[i] << " ";
    }
    cout<<endl;
    //load groundtruth
    int* queryGTlabel = ivecs_read("../data/sift/sift/label_sift_hard_groundtruth.ivecs", &k, &nq);
    int* queryGT = ivecs_read("../data/sift/sift/sift_groundtruth.ivecs", &k, &nq);
    for (int i; i<10; i++){
        cout<<queryGT[i]<<" ";
    }

    // recall

    
}

//*https://github.com/vaibhawvipul/performance-engineering/blob/main/matrix_multiplication/matrix_strassen_looporder_omp.cpp