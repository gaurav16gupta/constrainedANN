#pragma once
#include <x86intrin.h>
#include <iostream>

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

using namespace std;

float IP(float* a, float* b, size_t d){
    float ip=0;
    for(uint32_t k = 0; k < d; ++k) {    
        ip += a[k]*b[k]; // one unit FLOP- mul
    } 
    return ip;
}

double L2sim(float* a, float* b, float norm_bsq, size_t d){
    return (IP(a, b, d) -norm_bsq);
}

double L2Square(float* a, float* b, size_t d){
    double dist=0;
    for(uint32_t k = 0; k < d; ++k) {    
        dist += pow(a[k]-b[k],2); // two units FLOPS- mul and sub
    } 
    return dist;
}

double L2normSquare(float* a, size_t d){
    double norm=0;
    for(uint32_t k = 0; k < d; ++k) {    
        norm += a[k]*a[k]; // two units FLOPS- mul and sub
    } 
    return norm;
}

float IPSIMD4ExtAVX(float *pVect1, float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];
    size_t qty16 = qty / 16;
    size_t qty4 = qty / 4;
    const float *pEnd1 = pVect1 + 16 * qty16;
    const float *pEnd2 = pVect1 + 4 * qty4;
    __m256 sum256 = _mm256_set1_ps(0);
    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
        __m256 v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        __m256 v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum256 = _mm256_add_ps(sum256, _mm256_mul_ps(v1, v2));

        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum256 = _mm256_add_ps(sum256, _mm256_mul_ps(v1, v2));
    }

    __m128 v1, v2;
    __m128 sum_prod = _mm_add_ps(_mm256_extractf128_ps(sum256, 0), _mm256_extractf128_ps(sum256, 1));
    while (pVect1 < pEnd2) {
        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum_prod = _mm_add_ps(sum_prod, _mm_mul_ps(v1, v2));
    }
    _mm_store_ps(TmpRes, sum_prod);
    return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];
}

float L2SIMD4ExtAVX(float *pVect1, float *pVect2, float norm_bsq, size_t qty) {
    return (IPSIMD4ExtAVX(pVect1, pVect2, qty) -norm_bsq);
}
