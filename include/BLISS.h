// function to bins
#pragma once
#include <iostream>
#include <fstream>
#include <stdlib.h>    
#include <numeric>
#include <algorithm>
#include <string> 

// should call the bliss python training code
// make a clustering class and inherit it for faiss kmeans and bliss
using namespace std;
class BLISS
{
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
                hd[id] = IPSIMD16ExtAVX(input, weights+ id*(s1+1), s1);
                hd[id] += *(weights+ id*(s1+1) +s1); //bias
                if (hd[id]<0) hd[id] =0; // Relu 0 if negative
            }
            float* L1 = weights+ (s1+1)*s2;

            for (uint32_t id=0; id<s3; id++){
                float temp =0;
                last[id] = IPSIMD16ExtAVX(hd, L1+ id*(s2+1), s2); //multiply
                last[id] += *(L1+ id*(s2+1) +s2); //bias
            }
            // last = softmax(last);
        }

        uint32_t top(float* input){ 
            float* hd = new float[s2];
            for (uint32_t id=0; id<s2; id++){
                hd[id] = IPSIMD16ExtAVX(input, weights+ id*(s1+1), s1);
                hd[id] += *(weights+ id*(s1+1) +s1); //bias
                if (hd[id]<0) hd[id] =0; // Relu 0 if negative
            }
            float* L1 = weights+ (s1+1)*s2;
            uint32_t bin;
            float maxscore = -1000000; 
            for (uint32_t id=0; id<s3; id++){
                float temp =0;
                temp = IPSIMD16ExtAVX(hd, L1+ id*(s2+1), s2); //multiply
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

