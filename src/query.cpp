#include <iostream>
#include <fstream>
#include "FilterIndex.h"

// #define DATAPATH "/scratch/gg29/data/"

int main(int argc, char** argv)
{
    //default
    string metric = "L2";
    int mode = 0;
    string algo ="kmeans";
    size_t nc =0;
    // size_t buffer_size =0;
    size_t nprobe =0;

    size_t d, nb, nq, num_results; 
    string datapath, Attripath, querypath, queryAttripath, indexpath, GTpath;
    int k;
    int success = argkparser(argc, argv, &datapath, &Attripath, &querypath, &queryAttripath, &indexpath, &GTpath, &nc, &algo, &mode, &nprobe, &k);

    float* data = fvecs_read(datapath.c_str(), &d, &nb);
    vector<vector<string>> properties = getproperties(Attripath,' ');
    // nc = atoi(argv[2]); // num clusters
    FilterIndex myFilterIndex(data, d, nb, nc, properties, algo, mode);
    myFilterIndex.loadIndex(indexpath);
    cout << "Loaded" << endl;

    float* queryset = fvecs_read(querypath.c_str(), &d, &nq);
    vector<vector<string>> queryprops = getproperties(queryAttripath,' ');
    int* queryGTlabel = ivecs_read(GTpath.c_str(), &num_results, &nq);
    cout << "Query files read..." << endl;
    // nq = 10000;
    chrono::time_point<chrono::high_resolution_clock> t1, t2;
    t1 = chrono::high_resolution_clock::now();
    myFilterIndex.query(queryset, nq, queryprops, k, nprobe);
    t2 = chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;

    int32_t* output = myFilterIndex.neighbor_set;
    int output_[k*nq];
    copy(output, output+k*nq , output_);
    cout<<"numClusters, buffersize, QPS, Recall" << k << "@" << k << ":" <<endl;
    //QPS and recall
    double QPS;
    double recall = RecallAtK(queryGTlabel, output_, k, nq);
    printf("%d,%d,%f,%f\n",nc, nprobe, nq/diff.count(), recall);
}
