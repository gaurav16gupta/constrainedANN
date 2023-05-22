#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <set>
#include <iterator>
#include "readfile.h"
#include <stdlib.h>     /* calloc, exit, free */
#include <numeric>
#include <algorithm>
#include <string> 
#include <cstdint>
#include <map>
#include <cstdio>

#ifdef __AVX__
  #include <immintrin.h>
#else
  #warning AVX is not available. Code will not compile!
#endif

#if defined(__GNUC__)
#define PORTABLE_ALIGN32 __attribute__((aligned(32)))
#define PORTABLE_ALIGN64 __attribute__((aligned(64)))
#else
#define PORTABLE_ALIGN32 __declspec(align(32))
#define PORTABLE_ALIGN64 __declspec(align(64))
#endif

//include something for map
using namespace std;
int argparser(int argc, char** argv, string* basepath, string* labelpath, string* indexpath, size_t* nc, string* algo, int* mode);
int argparser(int argc, char** argv, string* basepath, string* labelpath, string* querypath, string* queryAttripath, string* indexpath, string* GTpath, size_t* nc, string* algo, int* mode, size_t* buffer_size);
double computeRecall(vector<vector<int>> answer, vector<vector<int>> guess);

float IP(float* a, float* b, size_t d);
double L2sim(float* a, float* b, float norm_bsq, size_t d);
double L2Square(float* a, float* b, size_t d);
double L2normSquare(float* a, size_t d);
float IPSIMD4ExtAVX(float *pVect1, float *pVect2, size_t qty);
float L2SIMD4ExtAVX(float *pVect1, float *pVect2, float norm_bsq, size_t qty);
// float IPSIMD16ExtAVX512(float *pVect1v,  float *pVect2v,  float *qty_ptr);
// float L2SIMD16ExtAVX512(float *pVect1, float *pVect2, float norm_bsq, size_t qty);
float spaseMul(uint8_t* prop, float* weight ,int na);
// to add AVXSIMD16 and SSE as well
uint16_t getclusterPart(uint16_t* maxMC, uint8_t* props, int treelen);
bool not_in(uint16_t x, uint16_t* a, int h);
double RecallAtK(int* answer, int* guess, size_t k, size_t nq);
void randomShuffle(int* v , int l, int u);
vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, uint32_t k, vector<float> topkDist);
