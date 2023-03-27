#include <iostream>
#include <fstream>
#include "FilterIndex.h"

int main(int argc, char** argv)
{
    if (argc != 5){
        std::cout << argv[0] << " dataname num_clusters numAttribites buffer_size"<< std::endl;
        exit(-1);
    }
    
    string datapath = "../data/" + string(argv[1]) + "/base.fvecs"; 
    string Attripath = "../data/" + string(argv[1]) + "/label_base_"+string(argv[3])+".txt"; 
    string querypath = "../data/" + string(argv[1]) + "/query.fvecs"; 
    string queryAttripath = "../data/" + string(argv[1]) + "/label_query_"+string(argv[3])+".txt"; 
    string GTpath = "../data/" + string(argv[1]) + "/label_"+string(argv[3])+"_hard_groundtruth.ivecs"; 
    string indexpath = string(argv[1])+ string(argv[2]) + string(argv[3]);
    
    string metric;
    if (string(argv[1])=="sift"){
        metric="L2";}
    if (string(argv[1])=="glove"){
        metric="Angular";}
    else{
        metric="L2";
    }

    size_t d, nb,nc, nq, num_results, buffer_size; 
    float* data = fvecs_read(datapath.c_str(), &d, &nb);
    vector<vector<string>> properties = getproperties(Attripath,' ');
    nc = atoi(argv[2]); // num clusters
    FilterIndex myFilterIndex(data, d, nb, nc, properties);
    cout<<"here0"<<endl;
    myFilterIndex.loadIndex(indexpath);
    cout << "Loaded" << endl;

    float* queryset = fvecs_read(querypath.c_str(), &d, &nq);
    vector<vector<string>> queryprops = getproperties(queryAttripath,' ');
    int* queryGTlabel = ivecs_read(GTpath.c_str(), &num_results, &nq);
    cout << "Query files read..." << endl;
    nq = 1000;
    buffer_size = atoi(argv[4]);

    chrono::time_point<chrono::high_resolution_clock> t1, t2;
    t1 = chrono::high_resolution_clock::now();
    myFilterIndex.query(queryset, nq, queryprops, num_results, buffer_size);
    t2 = chrono::high_resolution_clock::now();

    int32_t* output = myFilterIndex.neighbor_set;
    int output_[num_results*nq];
    copy(output, output+num_results*nq , output_);
    cout<<"numClusters, buffersize, QPS, Recall10@10 :"<<endl;
    //QPS and recall
    double QPS;
    double recall = RecallAtK(queryGTlabel, output_, num_results, nq);
    printf("%d,%d,%f,%f\n",nc, buffer_size, (double)1000000*nq/(double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count(), recall);
}
