#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "MurmurHash3.h"
#include "cbloomfilter.h"
using namespace std;

class BloomFilter
{
    public:
        BloomFilter(int m_value, int k_value);
        void insert(string keys[], int length);
        bool query(string q);

        int m;
        int k;
        int *bf;
};