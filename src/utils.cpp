
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

    // for (int i = 0; i < argc; ++i){
    //     if (std::strcmp("--Nc",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *nc = std::atoi(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --Nc"<<std::endl; 
    //             return -1;
    //         }
    //     }
    //     if (std::strcmp("--Algo",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *algo = std::string(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --Algo"<<std::endl; 
    //             return -1;
    //         }
    //     }
    //     if (std::strcmp("--mode",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *mode = std::atoi(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --mode"<<std::endl; 
    //             return -1;
    //         }
    //     }
    // }
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

    // for (int i = 0; i < argc; ++i){
    //     if (std::strcmp("--Nc",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *nc = std::atoi(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --Nc"<<std::endl; 
    //             return -1;
    //         }
    //     }
    //     if (std::strcmp("--Algo",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *algo = std::string(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --Algo"<<std::endl; 
    //             return -1;
    //         }
    //     }
    //     if (std::strcmp("--mode",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *mode = std::atoi(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --mode"<<std::endl; 
    //             return -1;
    //         }
    //     }
    //     if (std::strcmp("--Bf",argv[i]) == 0){
    //         if ((i+1) < argc){
    //             *buffer_size = std::atoi(argv[i+1]);
    //         } else {
    //             std::cerr<<"Invalid argument for optional parameter --Bf"<<std::endl; 
    //             return -1;
    //         }
    //     }
    // }
    return 0;
}


double RecallAtK(int* answer, int* guess, size_t k, size_t nq){
    uint32_t count = 0;
    for (int i=0;i<nq;i++){
        sort(answer+ k*i, answer + (i+1)*k);
        sort(guess+ k*i, guess+ (i+1)*k);
        std::vector<int> tmp;
        std::set_intersection(answer+ k*i, answer + (i+1)*k,  // Input iterators for first range 
                            guess+ k*i, guess+ (i+1)*k, // Input iterators for second range 
                            std::back_inserter(tmp));
        count += double(tmp.size());
    }
    return (count/double(nq*k));
}

float L2sim(float* a, float* b, float norm_bsq, size_t d){
    float dist=0;
    for(uint32_t k = 0; k < d; ++k) {    
        dist += a[k]*b[k]; // one unit FLOP- mul
        // dist += pow(a[k]-b[k],2); // two units FLOPS- mul and sub
    } 
    dist= dist- norm_bsq;
    return dist;
}

// Not giving speedup!! Check the issue
float L2SqrSIMD16ExtAVX(float *pVect1, float *pVect2, float norm_bsq, size_t qty) {
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
    float sum = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] - norm_bsq;
    return sum;
}

float IP(float* a, float* b, size_t d){
    float ip=0;
    for(uint32_t k = 0; k < d; ++k) {    
        ip += a[k]*b[k]; // one unit FLOP- mul
    } 
    return ip;
}
        
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

uint16_t getclusterPart(uint16_t* maxMC, vector<uint16_t> &props, int treelen){
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

//-----------------Unused functions--------------------------------------------------------------------------------------------------------------------------------


   // //check for logical errors
    // for (int i =counts[10]; i< counts[11]; i++){
    //     cout<<Lookup[i]<<" ";
    // }
    // cout<<endl;
    // for (int clID = 11; clID < 12; clID++){
    //     cout<<counts[clID]-counts[clID-1]<<endl; 
    // }

       // //check for logical errors
    // for (int clID = 10; clID < 11; clID++){
    //     for (int j=0; j< treelen+1; j++){
    //         int id = clID*(treelen+1) +j;
    //         cout<<maxMCIDs[id]<<endl;
    //         // for (int i =counts[j]; i< counts[j+1]; i++){
    //         //     cout<<Lookup[i]<<" ";}
    //         //     cout<<endl;
    //     }
    //     cout<<endl;
    // }

vector<int> argsort(int * query, int length)
{
    //A helper function to find the top num_results args in the coords closest to query
    //O(nk)
    vector<tuple<int, int>> sorted;
    for (int x = 0; x < length; x ++)
    {
        sorted.push_back({x, -1.0});
    }
    for (int index = 0; index < length; index++)
    {
        for (int order = 0; order < length; order++)
        {
            if (query[index] < get<1>(sorted.at(order)) || get<1>(sorted.at(order)) < 0)
            {
                sorted.insert(sorted.begin() + order, {index, query[index]});
                sorted.pop_back();
                break;
            }
        }
    }
    vector<int> nearest_neighbors;
    for (int index = 0; index < length; index++)
    {
        nearest_neighbors.push_back(get<0>(sorted.at(index)));
    }
    return nearest_neighbors;
}

vector<int> vectorArgsort(vector<double> query)
{
    //A helper function to find the top num_results args in the coords closest to query
    //O(nk)
    vector<tuple<int, double>> sorted;
    for (size_t x = 0; x < query.size(); x ++)
    {
        sorted.push_back({x, -1.0});
    }
    for (size_t index = 0; index < query.size(); index++)
    {
        for (size_t order = 0; order < query.size(); order++)
        {
            if (query[index] < get<1>(sorted.at(order)) || get<1>(sorted.at(order)) < 0)
            {
                sorted.insert(sorted.begin() + order, {index, query[index]});
                sorted.pop_back();
                break;
            }
        }
    }
    vector<int> nearest_neighbors;
    for (size_t index = 0; index < query.size(); index++)
    {
        nearest_neighbors.push_back(get<0>(sorted.at(index)));
    }
    return nearest_neighbors;
}


// save a load multiple vectors into the same bin file----
// bool save(const char * filename){
//     std::ofstream out(filename, std::ios::binary);
//     int a=A.size(), b=B.size(), c=C.size(), d=D.size(), e=E.size(), f=F.size();
//     out.write(reinterpret_cast<const char*>(&a), sizeof(a));
//     out.write(reinterpret_cast<const char*>(&b), sizeof(b));
//     out.write(reinterpret_cast<const char*>(&c), sizeof(c));
//     out.write(reinterpret_cast<const char*>(&d), sizeof(d));
//     out.write(reinterpret_cast<const char*>(&e), sizeof(e));
//     out.write(reinterpret_cast<const char*>(&f), sizeof(f));

//     out.write(reinterpret_cast<const char*>(&A[0]), sizeof(int)*A.size());
//     out.write(reinterpret_cast<const char*>(&B[0]), sizeof(int)*B.size());
//     out.write(reinterpret_cast<const char*>(&C[0]), sizeof(int)*C.size());
//     out.write(reinterpret_cast<const char*>(&D[0]), sizeof(int)*D.size());
//     out.write(reinterpret_cast<const char*>(&E[0]), sizeof(int)*E.size());
//     out.write(reinterpret_cast<const char*>(&F[0]), sizeof(int)*F.size());

//     // always check to see if the file opened, and if writes succeeded.  
//     // I am being lazy here so I can focus on the actual question
//     return true;
// }

// bool load(const char *filename){
//     std::ifstream in(filename, std::ios::binary);
//     int a, b, c, d, e, f;
//     in.read(reinterpret_cast<char*>(&a), sizeof(a));
//     in.read(reinterpret_cast<char*>(&b), sizeof(b));
//     in.read(reinterpret_cast<char*>(&c), sizeof(c));
//     in.read(reinterpret_cast<char*>(&d), sizeof(d));
//     in.read(reinterpret_cast<char*>(&e), sizeof(e));
//     in.read(reinterpret_cast<char*>(&f), sizeof(f));
//     A.resize(a); B.resize(b); C.resize(c); D.resize(d); E.resize(e); F.resize(f);

//     in.read(reinterpret_cast<char*>(&A[0]), sizeof(int)*A.size());
//     in.read(reinterpret_cast<char*>(&B[0]), sizeof(int)*B.size());
//     in.read(reinterpret_cast<char*>(&C[0]), sizeof(int)*C.size());
//     in.read(reinterpret_cast<char*>(&D[0]), sizeof(int)*D.size());
//     in.read(reinterpret_cast<char*>(&E[0]), sizeof(int)*E.size());
//     in.read(reinterpret_cast<char*>(&F[0]), sizeof(int)*F.size());

//     // always check to see if the file opened, and if writes succeeded.  
//     // I am being lazy here so I can focus on the actual question
//     return true;
// }



// vector<vector<int>> computeGroundTruth(vector<vector<int>> queryset, vector<set<string>> queryprops, vector<vector<int>> data, vector<set<string>> properties, int num_results)
// {
//     vector<vector<int>> answers;
//     for (int x = 0; x < queryset.size(); x++)
//     {
//         vector<int> answer;
//         set<string> props = queryprops[x];
//         vector<int> validIDs;
//         for (int y = 0; y < properties.size(); y++)
//         {
//             bool valid = true;
//             for (auto property : props)
//             {
//                 if (!properties[y].count(property))
//                 {
//                     valid = false;
//                 }
//             }
//             if (valid)
//             {
//                 validIDs.push_back(y);
//             }
//         }
//         cout << "num validIDs: " << validIDs.size() << endl;

//         vector<vector<int>> validCoords;
//         for (int a = 0; a < validIDs.size(); a++)
//         {
//             validCoords.push_back(data[validIDs[a]]);
//         }

//         cout << "num validCoords: " << validCoords.size() << endl;
        
//         vector<uint32_t> topArgs = argTopK(queryset[x], validCoords, num_results);

//         vector<uint32_t> argTopK(float* query, float* vectors, uint32_t d, uint32_t N, vector<uint32_t> idx, uint32_t idxSize, int k, float* topkDist){


//         cout << "topArgs: ";
//         for (int arg : topArgs)
//         {
//             cout  << arg << ", ";
//         }
//         cout << endl;

//         for (int z = 0; z < topArgs.size(); z++)
//         {
//             answer.push_back(validIDs[topArgs[z]]);
//         }
//         answers.push_back(answer);
//     }
//     return answers;
// }