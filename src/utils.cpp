
#include "utils.h"
#include <bits/stdc++.h>

using namespace std;

int argparser(int argc, char** argv, string* basepath, string* labelpath, string* indexpath, size_t* nc, string* algo, int* mode){
    if (argc < 4){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"./index <data> <properties> <outfile>";
        std::clog<<" [--Nc num_clusters] [--Algo method] [--mode method_version]"<<std::endl;

        std::clog<<"Positional arguments: "<<std::endl;
        std::clog<<"\t data: Filename pointing to an fvecs file (4 byte uint N, then list of  4 byte uint dim, then list of 32-bit little-endian floats)."<<std::endl;
        std::clog<<"\t properties: Filename pointing to a properties file (text file containing <num points> <num attributes> <newline> whitespace-separated property lists)"<<std::endl;
        // std::clog<<"\t space: Integer distance ID: 0 for L2 distance, 1 for inner product (angular distance)."<<std::endl;
        std::clog<<"\t outfile: folder for the index. Example : sift1024blissMode1, sift1024kmeans"<<std::endl;

        std::clog<<"Optional arguments: "<<std::endl;
        std::clog<<"\t [--Nc num_clusters]: (Optional, default 1024) Number of clusters/bins."<<std::endl;
        std::clog<<"\t [--Algo method]: (Optional, default kmeans) MEthod used for clustering. Use either bliss or kmeans"<<std::endl;
        std::clog<<"\t [--mode method_version]: (Optional, default 1) Only for bliss, use either 1,2 or 3. 1: embedding input - ANN labels, 2: embedding input - FilterANN labels, 3: embedding+Attribute input - FilterANN labels"<<std::endl;
        return -1;
    }

    // Positional arguments.
    *basepath = (string(argv[1])); 
    *labelpath = (string(argv[2]));
    *indexpath = (string(argv[3]));
    *nc = std::atoi(argv[4]);
    *algo = std::string(argv[5]);
    *mode = std::atoi(argv[6]);
    return 0;
}
int argparser(int argc, char** argv, string* basepath, string* labelpath, string* querypath, string* queryAttripath, string* indexpath, string* GTpath, size_t* nc, string* algo, int* mode, size_t* buffer_size){
    if (argc < 7){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"./index <data> <properties> <queries> <queryProperties> <index> <groundtruth>";
        std::clog<<" [--Nc num_clusters] [--Algo method] [--mode method_version]"<<std::endl;

        std::clog<<"Positional arguments: "<<std::endl;
        std::clog<<"\t data: Filename pointing to an fvecs file (4 byte uint N, then list of  4 byte uint dim, then list of 32-bit little-endian floats)."<<std::endl;
        std::clog<<"\t properties: Filename pointing to a properties file (text file containing <num points> <num attributes> <newline> whitespace-separated property lists)"<<std::endl;
        std::clog<<"\t queries: file for the queries."<<std::endl;
        std::clog<<"\t queryProperties: file for the queryProperties."<<std::endl;;
        std::clog<<"\t index: folder for the index. Example : sift1024blissMode1, sift1024kmeans"<<std::endl;
        std::clog<<"\t groundtruth: file for the groundtruth."<<std::endl;

        std::clog<<"Optional arguments: "<<std::endl;
        std::clog<<"\t [--Nc num_clusters]: (Optional, default 1024) Number of clusters/bins."<<std::endl;
        std::clog<<"\t [--Algo method]: (Optional, default kmeans) MEthod used for clustering. Use either bliss or kmeans"<<std::endl;
        std::clog<<"\t [--mode method_version]: (Optional, default 1) Only for bliss, use either 1,2 or 3. 1: embedding input - ANN labels, 2: embedding input - FilterANN labels, 3: embedding+Attribute input - FilterANN labels"<<std::endl;
        std::clog<<"\t [--Bf BufferSize]: (Optional, default 500) Number of distance computations."<<std::endl;

        return -1;
    }

    // Positional arguments.
    *basepath = (string(argv[1])); 
    *labelpath = (string(argv[2]));
    *querypath = (string(argv[3]));
    *queryAttripath = (string(argv[4])); 
    *indexpath = (string(argv[5]));
    *GTpath = (string(argv[6]));
    *nc = std::atoi(argv[7]);
    *algo = std::string(argv[8]);
    *mode = std::atoi(argv[9]);
    *buffer_size = std::atoi(argv[10]);
    return 0;
}

int argkparser(int argc, char** argv, string* basepath, string* labelpath, string* querypath, string* queryAttripath, string* indexpath, string* GTpath, size_t* nc, string* algo, int* mode, size_t* buffer_size, int* k){
    if (argc < 8){
        std::clog<<"Usage: "<<std::endl; 
        std::clog<<"./index <data> <properties> <queries> <queryProperties> <index> <groundtruth>";
        std::clog<<" [--Nc num_clusters] [--Algo method] [--mode method_version]"<<std::endl;

        std::clog<<"Positional arguments: "<<std::endl;
        std::clog<<"\t data: Filename pointing to an fvecs file (4 byte uint N, then list of  4 byte uint dim, then list of 32-bit little-endian floats)."<<std::endl;
        std::clog<<"\t properties: Filename pointing to a properties file (text file containing <num points> <num attributes> <newline> whitespace-separated property lists)"<<std::endl;
        std::clog<<"\t queries: file for the queries."<<std::endl;
        std::clog<<"\t queryProperties: file for the queryProperties."<<std::endl;;
        std::clog<<"\t index: folder for the index. Example : sift1024blissMode1, sift1024kmeans"<<std::endl;
        std::clog<<"\t groundtruth: file for the groundtruth."<<std::endl;

        std::clog<<"Optional arguments: "<<std::endl;
        std::clog<<"\t [--Nc num_clusters]: (Optional, default 1024) Number of clusters/bins."<<std::endl;
        std::clog<<"\t [--Algo method]: (Optional, default kmeans) MEthod used for clustering. Use either bliss or kmeans"<<std::endl;
        std::clog<<"\t [--mode method_version]: (Optional, default 1) Only for bliss, use either 1,2 or 3. 1: embedding input - ANN labels, 2: embedding input - FilterANN labels, 3: embedding+Attribute input - FilterANN labels"<<std::endl;
        std::clog<<"\t [--Bf BufferSize]: (Optional, default 500) Number of distance computations."<<std::endl;

        return -1;
    }

    // Positional arguments.
    *basepath = (string(argv[1])); 
    *labelpath = (string(argv[2]));
    *querypath = (string(argv[3]));
    *queryAttripath = (string(argv[4])); 
    *indexpath = (string(argv[5]));
    *GTpath = (string(argv[6]));
    *nc = std::atoi(argv[7]);
    *algo = std::string(argv[8]);
    *mode = std::atoi(argv[9]);
    *buffer_size = std::atoi(argv[10]);
    *k = std::atoi(argv[11]);
    return 0;
}

double RecallAtK(int* answer, int* guess, size_t k, size_t nq){
    uint32_t count = 0;
    for (int i=0;i<nq;i++){
        // Editing here
        sort(answer+ 100*i, answer + 100*i+k);
        sort(guess+ k*i, guess+ (i+1)*k);
        std::vector<int> tmp;
        std::set_intersection(answer+ 100*i, answer + 100*i+k,  // Input iterators for first range 
                            guess+ k*i, guess+ (i+1)*k, // Input iterators for second range 
                            std::back_inserter(tmp));
        count += double(tmp.size());
    }
    return (count/double(nq*k));
}

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

float spaseMul(uint8_t* prop, float* weight ,int na){
    float sum=0;
    for (size_t i=0; i<na; i++){
        sum+=weight[prop[i]];
    }
    return sum;
}

// float IPSIMD16ExtAVX512(float *pVect1,  float *pVect2,  size_t qty) {
//     float PORTABLE_ALIGN64 TmpRes[16];
//     size_t qty16 = qty / 16;
//     const float *pEnd1 = pVect1 + 16 * qty16;
//     __m512 sum512 = _mm512_set1_ps(0);
//     while (pVect1 < pEnd1) {
//         //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
//         __m512 v1 = _mm512_loadu_ps(pVect1);
//         pVect1 += 16;
//         __m512 v2 = _mm512_loadu_ps(pVect2);
//         pVect2 += 16;
//         sum512 = _mm512_add_ps(sum512, _mm512_mul_ps(v1, v2));
//     }
//     _mm512_store_ps(TmpRes, sum512);
//     float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] + TmpRes[8] + TmpRes[9] + TmpRes[10] + TmpRes[11] + TmpRes[12] + TmpRes[13] + TmpRes[14] + TmpRes[15];
//     return sum;
// }

// float L2SIMD16ExtAVX512(float *pVect1, float *pVect2, float norm_bsq, size_t qty) {
//     return (IPSIMD16ExtAVX512(pVect1, pVect2, qty) -norm_bsq);
// }


uint16_t getclusterPart(uint16_t* maxMC, uint8_t* props, int treelen){
    // maxMC: property location, property, frequency
    for (uint16_t i=0;i<treelen; i++){
        if (maxMC[i*3+1] == props[maxMC[i*3+0]]){
            return i;
        }
    }
    return treelen;    
}

//checks if the property x is seen before in maxMC
bool not_in(uint16_t x, uint16_t* a, int h){
    // property location, property, frequency
    if (h == 0){
        return 1;
    } 
    else{
        for(uint16_t i=0;i< h;i++){ 
            if (a[i*3+1]==x){return 0;}
        };
        return 1;
    }
}

void randomShuffle(int* v , int l, int u){
     // Range of numbers [l, u]
    iota(v, v+u-l, l); 
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v, v+u-l, g);
}


vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, uint32_t k, vector<float> topkDist){
    float dist; 
    vector<uint32_t> topk;
    priority_queue<pair<float, uint32_t> > pq;
    if (idxSize ==N){
        for (uint32_t i = 0; i < N; i++){
            //L2
            dist =0;
            for (size_t j = 0; j < d; j++){
                dist += pow(vectors[i*d+j] - query[j], 2);
            }
            dist = sqrt(dist);
            //topk
            if (i<k) pq.push({dist, i});
            else{
                if (dist< pq.top().first){
                    pq.pop();
                    pq.push({dist, i});
                }
            }
        }
    }
    else{
        for (uint32_t i = 0; i < idxSize; i++){
            //L2
            try{
                dist =0;
                for (size_t j = 0; j < d; j++){
                    dist += pow(vectors[idx[i]*d+j] - query[j], 2);//*
                }

                dist = sqrt(dist);
                //topk
                if (i<k) pq.push({dist, idx[i]});
                else{
                    if (dist< pq.top().first){
                        pq.pop();
                        pq.push({dist, idx[i]});
                    }
                }
            }
            catch(int mynum){
                cout << "Error number: "; 
            }
        }
    }
    for (uint32_t i = 0; i < k; i++){
        topk.push_back(pq.top().second);
        topkDist.push_back(pq.top().first);
        pq.pop();
    }
    return topk;
}
