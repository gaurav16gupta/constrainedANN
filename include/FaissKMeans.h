// function to bins
#pragma once
#include <iostream>
#include <fstream>
#include <stdlib.h>    
#include <numeric>
#include <algorithm>
#include <string> 

#include <faiss/Clustering.h> 
#include <faiss/IndexFlat.h>
#include <bits/stdc++.h>
#include "utils.h"

using namespace std;

class Kmeans
{
    public:
        int nc;
        int d;
        float* centroids;
        float* cen_norms;
        Kmeans(int nc, int d): nc(nc), d(d){
            centroids = new float[d*nc]; //provide random nc vectors
            cen_norms = new float[nc]{0};
        }

        void train(float* dataset, int nb){
            //centroids (nc * d) if centroids are set on input to train, they will be used as initialization
            int v[nb];
            randomShuffle(v , 0, nb);
            faiss::Clustering clus(d, nc);
            clus.centroids.resize(d*nc);
            for(uint32_t i = 0; i < nc; ++i) { 
                for(uint32_t j = 0; j < d; ++j){
                    clus.centroids[i*d+j] = dataset[v[i]*d +j];
                }
                // memcpy(clus.centroids + i*d, dataset +v[i]*d, sizeof(*centroids) * d);
            }
            clus.verbose = d * nb * nc > (1L << 30);
            // display logs if > 1Gflop per iteration

            faiss::IndexFlatL2 index(d);
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
        }

        uint32_t top(float* input){
            uint32_t bin;
            float minscore = 1000000;  
            for(uint32_t j = 0; j < nc; ++j){  
                float temp =0;
                for(uint32_t k = 0; k < d; ++k) {                 
                    temp += pow(input[k] - centroids[j*d +k], 2);   //change this to L2 dist function    
                } 
                if (temp<minscore) {
                    minscore=temp;
                    bin = j;}    
            }
            return bin;
        }

        float* score(float* input, float* scores){
            for (uint32_t id=0; id<nc; id++){
                scores[id] = L2SqrSIMD16ExtAVX(input, centroids+id*d, cen_norms[id], d);
            }
            return scores;
        }

        void save(string indexpath){
            FILE* f1 = fopen((indexpath+"/centroids.bin").c_str(), "wb");
            fwrite(centroids, sizeof(float), nc*d, f1);
            fclose (f1);

            FILE* f2 = fopen((indexpath+"/centroidsNorms.bin").c_str(), "wb");
            fwrite(cen_norms, sizeof(float), nc, f2);
            fclose (f2);
        }

        void load(string indexpath){
            FILE* f1 = fopen((indexpath+"/centroids.bin").c_str(), "rb");
            fread(centroids, sizeof(float), nc*d, f1);   
            fclose (f1);    
            FILE* f2 = fopen((indexpath+"/centroidsNorms.bin").c_str(), "rb");
            fread(cen_norms, sizeof(float), nc, f2);
            fclose (f2);
        } 
};


