#pragma once
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

//include something for map
using namespace std;

double computeRecall(vector<vector<int>> answer, vector<vector<int>> guess);
vector<vector<int>> computeGroundTruth(vector<vector<int>> queryset, vector<set<string>> queryprops, vector<vector<int>> data, vector<set<string>> properties, int num_results);
vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, uint32_t k, vector<float> topkDist);
float L2sim(float* a, float* b, float norm_bsq, size_t d);
double RecallAtK(int* answer, int* guess, size_t k, size_t nq);

vector<int> argsort(int * query, int length);
vector<int> vectorArgsort(vector<double> query);

void randomShuffle(int* v , int l, int u);

