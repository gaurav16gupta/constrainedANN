#pragma once

#ifndef MLP_H
#define MLP_H

#include <vector>
using namespace std;

class MLP {
    public:
        MLP(string modelpath);
        float* forward(float* input);
        double sigmoid(double x);
        float IP(float* a, float* b, size_t d);
        float IPSIMD16ExtAVX(float *pVect1, float *pVect2, size_t qty);

    private:
        uint16_t s1;
        uint16_t s2;
        uint16_t s3;
};

#endif

