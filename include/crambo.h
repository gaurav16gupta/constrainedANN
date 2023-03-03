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
#include <stdlib.h>     /* calloc, exit, free */
#include <algorithm>
#include <string> 
#include <cstdint>
using namespace std;


class RAMBO
{
    public:
        RAMBO(int b_value, int r_value);
        int* insert(set<string> *keys[], int length, int lengths[]);
        set<int> query(string q, int* setIDs);

        int b;
        int r;
        int n;
        uintptr_t* bloomFilters;
};
