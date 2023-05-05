#include "mlp.h"
#include <cmath>

MLP::MLP(string modelpath){
    // load .dat
    ifstream file(modelpath, ios::binary);
    if (!file.is_open()) cout << "Failed to open " << modelpath<< endl;
    // get the size of the file
    file.seekg(0, ios::end);
    int fileSize = file.tellg();
    file.seekg(0, ios::beg);
    // dynamically allocate memory for the buffer
    float* weights = new float[fileSize];
    // read the contents of the file into the buffer
    file.read(weights, fileSize);    
    // close the file
    file.close();

    S1 = weights[0]; // todo:change the model files accordingly
    s2 = weights[1];
    s3 = weights[2];
}

float* MLP::forward(float* input) {
    // Compute hidden layer values
    weights = weights+3;
    float* hd = new float[s2];
    for (uint32_t id=0; id<s2; id++){
        hd[id] = IP(input, weights+ id*(s1+1), s1); //multiply
        hd[id] = hd[id]+ *(weights+ id*(s1+1) +s1); //bias
        if (hd[id]<0) hd[id] =0; // Relu 0 if negative
    }

    weights = weights+ (s1+1)*s2;
    float* last = new float[s3];
    for (uint32_t id=0; id<s3; id++){
        last[id] = IP(hd, weights+3+ id*(s2+1), s2); //multiply
        last[id] = last[id]+ *(weights+3+ id*(s2+1) +s2); //bias
    }
    // last = softmax(last);
    return last;
}

double MLP::sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}


float IP(float* a, float* b, size_t d){
    float ip=0;
    for(uint32_t k = 0; k < d; ++k) {    
        ip += a[k]*b[k]; // one unit FLOP- mul
    } 
    return ip;
}

// todo: Not giving speedup!! Check the issue
float IPSIMD16ExtAVX(float *pVect1, float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];
    size_t qty16 = qty / 16;
    const float *pEnd1 = pVect1 + 16 * qty16;
    __m256 sum256 = _mm256_set1_ps(0);
    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
        const __m256 v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        const __m256 v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum256 = _mm256_add_ps(sum256, _mm256_mul_ps(v1, v2));
    }
    _mm256_store_ps(TmpRes, sum256);
    float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7];
    return sum;
}

