#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <set>
#include <iterator>
using namespace std;

#include <fstream>

vector<vector<string>> getproperties(string fileName, char dlim);
vector<vector<int>> coordinates(string fileName);
float* fvecs_read(const char* fname, size_t* d_out, size_t* n_out);
int* ivecs_read(const char* fname, size_t* d_out, size_t* n_out);
