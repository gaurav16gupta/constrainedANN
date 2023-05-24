// function to bins
#pragma once
#include <iostream>
#include <fstream>
#include <stdlib.h>    
#include <numeric>
#include <algorithm>
#include <string> 
#include "utils.h"

//faiss specific imports
#include <faiss/Clustering.h> 
#include <faiss/IndexFlat.h>
#include <bits/stdc++.h>

using namespace std;
class cluster{
    public:
        virtual void train(float* dataset, int nb, string modelpath) = 0;
        virtual void getscore(float* input, float* last) = 0;
        virtual uint32_t top(float* input) = 0;
        virtual void load(string modelpath) = 0;
};

// add any other cluster method as a seperate derived class
class BLISS : public cluster{
    public:
        int s1;
        int s2;
        int s3;
        float* weights;
        
        BLISS(int s1, int s2, int s3): s1(s1), s2(s2), s3(s3){
            weights = new float[(s2*(s1+1) +s3*(s2+1))]; 
        }

        void train(float* dataset, int nb, string modelpath){
            // load .bin
            FILE* f = fopen((modelpath + "/model.bin").c_str(), "rb");
            fread(weights, sizeof(float), (s2*(s1+1) +s3*(s2+1)), f); 
            fclose (f);
        } 

        void getscore(float* input, float* last){
            float* hd = new float[s2];
            for (uint32_t id=0; id<s2; id++){
                hd[id] = IPSIMD4ExtAVX(input, weights+ id*(s1+1), s1);
                hd[id] += *(weights+ id*(s1+1) +s1); //bias
                if (hd[id]<0) hd[id] =0; // Relu 0 if negative
            }
            float* L1 = weights+ (s1+1)*s2;

            for (uint32_t id=0; id<s3; id++){
                float temp =0;
                last[id] = IPSIMD4ExtAVX(hd, L1+ id*(s2+1), s2); //multiply
                last[id] += *(L1+ id*(s2+1) +s2); //bias
            }
            // last = softmax(last);
        }

        uint32_t top(float* input){ 
            float* hd = new float[s2];
            for (uint32_t id=0; id<s2; id++){
                hd[id] = IPSIMD4ExtAVX(input, weights+ id*(s1+1), s1);
                hd[id] += *(weights+ id*(s1+1) +s1); //bias
                if (hd[id]<0) hd[id] =0; // Relu 0 if negative
            }
            float* L1 = weights+ (s1+1)*s2;
            uint32_t bin;
            float maxscore = -1000000; 
            for (uint32_t id=0; id<s3; id++){
                float temp =0;
                temp = IPSIMD4ExtAVX(hd, L1+ id*(s2+1), s2); //multiply
                temp += *(L1+ id*(s2+1) +s2); //bias
                if (temp>maxscore) {
                maxscore=temp;
                bin = id;}
            }
            return bin;
        }

        void load(string modelpath){
            // load .bin
            FILE* f = fopen((modelpath + "/model.bin").c_str(), "rb");
            fread(weights, sizeof(float), (s2*(s1+1) +s3*(s2+1)), f); 
            fclose (f);
        } 
};

class Kmeans: public cluster{
    public:
        int nc;
        int d;
        float* centroids;
        float* cen_norms;
        Kmeans(int nc, int d): nc(nc), d(d){
            centroids = new float[d*nc]; //provide random nc vectors
            cen_norms = new float[nc]{0};
        }

        void train(float* dataset, int nb, string modelpath){
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

            FILE* f1 = fopen((modelpath+"/centroids.bin").c_str(), "wb");
            fwrite(centroids, sizeof(float), nc*d, f1);
            fclose (f1);

            FILE* f2 = fopen((modelpath+"/centroidsNorms.bin").c_str(), "wb");
            fwrite(cen_norms, sizeof(float), nc, f2);
            fclose (f2);
        }

        uint32_t top(float* input){
            uint32_t bin=0;
            float maxscore = L2SIMD4ExtAVX(input, centroids, cen_norms[0], d);;  
            float temp;
            for(uint32_t j = 1; j < nc; ++j){  
                temp = L2SIMD4ExtAVX(input, centroids+j*d, cen_norms[j], d);
                // for(uint32_t k = 0; k < d; ++k) {                 
                //     temp += pow(input[k] - centroids[j*d +k], 2);   //change this to L2 dist function    
                // } 
                if (temp>maxscore) {
                    maxscore=temp;
                    bin = j;}    
            }
            return bin;
        }
        
        void getscore(float* input, float* scores){
            for (uint32_t id=0; id<nc; id++){
                scores[id] = L2SIMD4ExtAVX(input, centroids+id*d, cen_norms[id], d);
            }
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

// add any other cluster method as a seperate derived class
class BLISSmode3 : public cluster{
    public:
        int s1;
        int sa; //size of attribute vocab
        int na;
        int s2;
        int s3;
        float* weights;
        
        BLISSmode3(int s1, int s2, int s3, int na): s1(s1), s2(s2), s3(s3), sa(sa), na(na){
            weights = new float[(s2*(s1+sa+1) +s3*(s2+1))]; 
        }

        void train(float* dataset, int nb, string modelpath){
            // load .bin
            FILE* f = fopen((modelpath + "/model.bin").c_str(), "rb");
            fread(weights, sizeof(float), (s2*(s1+sa+1) +s3*(s2+1)), f); 
            fclose (f);
        } 
        
        void getscore(float* input, uint8_t* prop, float* last){
            float* hd = new float[s2];
            for (uint32_t id=0; id<s2; id++){
                hd[id] = IPSIMD4ExtAVX(input, weights+ id*(s1+sa+1), s1) + spaseMul(prop, weights+ id*(s1+sa+1),na);
                hd[id] += *(weights+ id*(s1+sa+1) +s1+sa); //bias
                if (hd[id]<0) hd[id] =0; // Relu 0 if negative
            }
            float* L1 = weights+ (s1+sa +1)*s2;

            for (uint32_t id=0; id<s3; id++){
                float temp =0;
                last[id] = IPSIMD4ExtAVX(hd, L1+ id*(s2+1), s2); //multiply
                last[id] += *(L1+ id*(s2+1) +s2); //bias
            }
            // last = softmax(last);
        }

        uint32_t top(float* input, uint8_t* prop){ 
            float* hd = new float[s2];
            for (uint32_t id=0; id<s2; id++){
                hd[id] = IPSIMD4ExtAVX(input, weights+ id*(s1+sa+1), s1) + spaseMul(prop, weights+ id*(s1+sa+1),na);
                hd[id] += *(weights+ id*(s1+sa+1) +s1+sa); //bias
                if (hd[id]<0) hd[id] =0; // Relu 0 if negative
            }
            float* L1 = weights+ (s1+sa+1)*s2;
            uint32_t bin;
            float maxscore = -1000000; 
            for (uint32_t id=0; id<s3; id++){
                float temp =0;
                temp = IPSIMD4ExtAVX(hd, L1+ id*(s2+1), s2); //multiply
                temp += *(L1+ id*(s2+1) +s2); //bias
                if (temp>maxscore) {
                maxscore=temp;
                bin = id;}
            }
            return bin;
        }

        void load(string modelpath){
            // load .bin
            FILE* f = fopen((modelpath + "/model.bin").c_str(), "rb");
            fread(weights, sizeof(float), (s2*(s1+sa+1) +s3*(s2+1)), f); 
            fclose (f);
        } 
};


