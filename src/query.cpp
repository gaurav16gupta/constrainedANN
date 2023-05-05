#include <iostream>
#include <fstream>
#include "FilterIndex.h"

#define DATAPATH "/scratch/gg29/data/"

int main(int argc, char** argv)
{
    if (argc != 6){
        std::cout << argv[0] << " dataname num_clusters numAttribites buffer_size mode"<< std::endl;
        exit(-1);
    }
    string algo = "bliss"; // or bliss1, bliss2, bliss3

    string datapath = DATAPATH + string(argv[1]) + "/base.fvecs"; 
    string Attripath = DATAPATH + string(argv[1]) + "/label_base_"+string(argv[3])+".txt"; 
    string querypath = DATAPATH + string(argv[1]) + "/query.fvecs"; 
    string queryAttripath = DATAPATH + string(argv[1]) + "/label_query_"+string(argv[3])+".txt"; 
    string GTpath = DATAPATH + string(argv[1]) + "/label_"+string(argv[3])+"_hard_groundtruth.ivecs"; 
    string indexpath = "indices/"+ string(argv[1])+ string(argv[2]) +algo+ "Mode" + string(argv[5]) ;
    cout<<indexpath<<endl;
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
    FilterIndex myFilterIndex(data, d, nb, nc, properties, algo);
    myFilterIndex.loadIndex(indexpath);
    cout << "Loaded" << endl;

    float* queryset = fvecs_read(querypath.c_str(), &d, &nq);
    vector<vector<string>> queryprops = getproperties(queryAttripath,' ');
    int* queryGTlabel = ivecs_read(GTpath.c_str(), &num_results, &nq);
    cout << "Query files read..." << endl;
    nq = 10000;
    buffer_size = atoi(argv[4]);

    chrono::time_point<chrono::high_resolution_clock> t1, t2;
    t1 = chrono::high_resolution_clock::now();
    
    myFilterIndex.query(queryset, nq, queryprops, num_results, buffer_size);
    t2 = chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;


    int32_t* output = myFilterIndex.neighbor_set;
    int output_[num_results*nq];
    copy(output, output+num_results*nq , output_);
    cout<<"numClusters, buffersize, QPS, Recall100@100 :"<<endl;
    //QPS and recall
    double QPS;
    double recall = RecallAtK(queryGTlabel, output_, num_results, nq);
    printf("%d,%d,%f,%f\n",nc, buffer_size, nq/diff.count(), recall);
}
