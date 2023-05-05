#include <iostream>

#ifdef __AVX__
  #include <immintrin.h>
#else
  #warning No AVX support - will not compile
#endif


void L2SqrSIMD16ExtAVX512(float *d) {
  __m256 a = _mm256_set_ps(8.0, 7.0, 6.0, 5.0, 
                             4.0, 3.0, 2.0, 1.0);
    __m256 b = _mm256_set_ps(18.0, 17.0, 16.0, 15.0, 
                             14.0, 13.0, 12.0, 11.0);

    __m256 c = _mm256_add_ps(a, b);

    
    _mm256_storeu_ps(d, c);
}


int main(int argc, char **argv)
{ 
    float d[8];
    L2SqrSIMD16ExtAVX512(d);
    std::cout << "result equals " << d[0] << "," << d[1]
              << "," << d[2] << "," << d[3] << ","
              << d[4] << "," << d[5] << "," << d[6] << ","
              << d[7] << std::endl;

    return 0;
}